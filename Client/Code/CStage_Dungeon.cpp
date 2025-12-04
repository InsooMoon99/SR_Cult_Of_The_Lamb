#include "pch.h"
#include "CStage_Dungeon.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CHumanMonster.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSceneMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIItemGetManager.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUITabMenu.h"
#include "CUIWeapon.h"
#include "ItemInstance.h"

CStage_Dungeon::CStage_Dungeon(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_Terrain(nullptr), m_CurrentRoom(Room::DUNGEON_END)
{
}

CStage_Dungeon::~CStage_Dungeon()
{ }

HRESULT CStage_Dungeon::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    // BGM Test
    // CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Intro.wav", 0.5f);

    return S_OK;
}

void CStage_Dungeon::Initialize_Scene()
{
    CScene::Initialize_Scene();

    // 방 위치 일단 대충 설정
    m_RoomData[(int)Room::START] = { {20.f, 0.f, 20.f}, {20.f, 1.f, 20.f}, 30.f, 30.f };
    m_RoomData[(int)Room::MONSTER1] = { {70.f, 0.f, 20.f}, {70.f, 1.f, 20.f}, 30.f, 30.f };
    m_RoomData[(int)Room::MONSTER2] = { {120.f, 0.f, 20.f}, {120.f, 1.f, 20.f}, 30.f, 30.f };
    m_RoomData[(int)Room::TARO] = { {170.f, 0.f, 20.f}, {170.f, 1.f, 20.f}, 30.f, 30.f };
    m_RoomData[(int)Room::BOSS] = { {220.f, 0.f, 20.f}, {220.f, 1.f, 20.f}, 40.f, 40.f };

    m_CurrentRoom = Room::START;

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Dungeon1.map");
}

_int CStage_Dungeon::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS1);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_G))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS2);

    return exit;
}

void CStage_Dungeon::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Dungeon::Render_Scene()
{
    // debug 용
}

void CStage_Dungeon::Spawn_RoomObjects(Room room)
{
    switch (room)
    {
    case Room::START:
        Spawn_Weapon();
        break;
    case Room::MONSTER1:
        Spawn_Monster(3);
        break;
    case Room::MONSTER2:
        Spawn_Monster(5);
        break;
    case Room::TARO:
        // 타로 NPC 소환
        break;
    case Room::BOSS:

        break;
    }
}

void CStage_Dungeon::Spawn_Weapon()
{
    // 일단 무기 2개만 스폰
    CDropSystem::GetInstance()->SetCurrentScene(this);

    _int size = CItemDB::GetInstance()->GetVector().size();
    for (_int i = 0; i < 2; i++)
    {
        int id = CItemDB::GetInstance()->GetVector()[size - i - 1].id;

        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 15.f, 1.f, 10.f + (i * 3.f) });
    }
}

void CStage_Dungeon::Spawn_Monster(_int index)
{

}

void CStage_Dungeon::Load_Room(Room room)
{

}

void CStage_Dungeon::unloadRoom(Room room)
{

}

HRESULT CStage_Dungeon::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    m_Terrain = CTerrain::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_Terrain, E_FAIL);

    layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrain);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer,
        E_FAIL,
        L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CLayer::Create() returned null");

    Engine::CGameObject* gameObject = nullptr;

    // -----------------------------
    // Player
    // -----------------------------

    // 플레이어 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CPlayer>(
            OBJTYPE::PLAYER,
            L"Player",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");

    NULL_CHECK_RETURN(gameObject, E_FAIL);

    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    NULL_CHECK_RETURN(playerTransform, E_FAIL);

    m_PlayerInventoryCom =
        dynamic_cast<CInventory*>(gameObject->Get_Component(
            ID_STATIC,
            COMPONENTTYPE::INVENTORY));

    NULL_CHECK_RETURN(m_PlayerInventoryCom, E_FAIL);

    m_PlayerCombatStatCom =
        dynamic_cast<CCombatStat*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::COMBATSTAT));

    NULL_CHECK_RETURN(m_PlayerCombatStatCom, E_FAIL);

    // -----------------------------
    // Monster
    // -----------------------------

    // HumanMonster
    gameObject = CHumanMonster::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CHumanMonster::Create() returned null")

        FAILED_CHECK_MSG(
            layer->Add_GameObject(OBJTYPE::HUMANMONSTER, gameObject),
            L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed");

    auto camController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (camController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(camController)
            .SetTarget(playerTransform)
            //.PivotOffset({ 0.f, 0.f, 0.f })
            //.Distance(12.f)
            //.RotX(45.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(camController);
    }

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CGameObject* gameObject = nullptr;

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIPlayerPanel>(
            OBJTYPE::UI,
            L"UI_PlayerPanel",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_PlayerPanel");

    CUIPlayerPanel* playerPanel = dynamic_cast<CUIPlayerPanel*>(gameObject);

    playerPanel->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    playerPanel->Set_Resister(true);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIMp>(
            OBJTYPE::UI,
            L"UI_Mp",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Mp");

    CUIMp* uiMp = dynamic_cast<CUIMp*>(gameObject);
    uiMp->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUICircle>(
            OBJTYPE::UI,
            L"UI_Circle",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Circle");

    dynamic_cast<CUICircle*>(gameObject)->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIWeapon>(
            OBJTYPE::UI,
            L"UI_Weapon",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Weapon");

    //왼쪽 상단 스킬 아이콘 UI
    CUIIcon* pgameObject = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    pgameObject->Set_TextureIndex(1);
    pgameObject->Get_Transform()->Set_Pos(-524.f, 260.f, 0.f);
    pgameObject->Get_Transform()->Set_Scale(12.5f, 15.f, 0.f);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pgameObject),
        L"CStage_Dungeon::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    //CUI_TabMenu
    CUITabMenu* uiTabMenu = CUITabMenu::Create(m_GraphicDev);
    uiTabMenu->SetInventory(m_PlayerInventoryCom);
    uiTabMenu->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    uiTabMenu->Initialize();

    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiTabMenu),
        L"CStage_Dungeon::Ready_UI_Layer() failed: CLayer::Add_GameObject(UITabMenu) failed");


    FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGetManager>
        (OBJTYPE::UI, L"UI_ItemGetManager",
            m_GraphicDev, layer,
            &gameObject),
        L"Persistent object setup failed: UI_ItemGetManager")
        dynamic_cast<CUIItemGetManager*>(gameObject)->SetInventory(m_PlayerInventoryCom);
    dynamic_cast<CUIItemGetManager*>(gameObject)->Initialize();

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_Prototype()
{
    return S_OK;
}

CStage_Dungeon* CStage_Dungeon::Create(DEVICE graphicDev)
{
    auto stage = new CStage_Dungeon(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_Dungeon::Free()
{
    Engine::CScene::Free();
}
