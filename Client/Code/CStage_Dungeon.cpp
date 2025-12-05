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
#include "CTrigger_Dungeon.h"

CStage_Dungeon::CStage_Dungeon(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_CurrentRoom(Room::DUNGEON_END), m_Player(nullptr), m_PlayerTransform(nullptr)
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

    m_RoomData[static_cast<_int>(Room::START)] =
    {
        {10.f, 1.f, 10.f},                          
        "../Bin/Resource/Data/Terrain/Terrain_Dungeon1.map",     
        {0.f, 0.f, 0.f}                                   
    };

    m_RoomData[static_cast<_int>(Room::MONSTER1)] =
    {
        {60.f, 1.f, 10.f},
        "../Bin/Resource/Data/Terrain/Terrain_Mon1Test.map",
        {50.f, 0.f, 0.f}
    };

    m_RoomData[static_cast<_int>(Room::MONSTER2)] =
    {
        {110.f, 1.f, 10.f},
        "../Bin/Resource/Data/Terrain/Terrain_Mon2Test.map",
        {100.f, 0.f, 0.f}
    };

    m_RoomData[static_cast<_int>(Room::TARO)] =
    {
        {160.f, 1.f, 10.f},
        "../Bin/Resource/Data/Terrain/Terrain_TaroTest.map",
        {150.f, 0.f, 0.f}
    };

    m_RoomData[static_cast<_int>(Room::BOSS)] =
    {
        {210.f, 1.f, 10.f},
        "../Bin/Resource/Data/Terrain/Terrain_BossTest.map",
        {200.f, 0.f, 0.f}
    };

    m_CurrentRoom = Room::START;

    for (auto& pair : m_Terrains)
    {
        Room room = pair.first;
        CTerrain* terrain = pair.second;

        if (terrain)
        {
            auto terrainTransform = terrain->Get_Transform();
            if (terrainTransform)
            {
                terrainTransform->Set_Pos(m_RoomData[static_cast<_int>(room)].terrainOffset);
            }

            terrain->LoadMapOnStart(m_RoomData[static_cast<_int>(room)].terrainFilePath.c_str());

        }
    }

    // 트리거 생성
    Create_RoomTriggers();
}

_int CStage_Dungeon::Update_Scene(const _float& timeDelta)
{
    if (m_RequestChangeScene)
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(m_NextSceneType);
        m_RequestChangeScene = false;

        return 0;
    }

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
    // TODO : 몬스터 소환, 그런데 그냥 에디터에서 세팅할 수 있게 해버릴지 ?

}

void CStage_Dungeon::UnLoad_Room(Room room)
{

}

void CStage_Dungeon::Load_Room(Room room)
{

}

void CStage_Dungeon::Create_RoomTriggers()
{
    // START -> MONSTER1
    Create_Trigger_Between(
        Room::START,
        Room::MONSTER1,
        _vec3(18.f, 1.f, 27.f),     
        _vec3(5.f, 5.f, 4.5f)      
    );

    // MONSTER1 -> MONSTER2
    Create_Trigger_Between(
        Room::MONSTER1,
        Room::MONSTER2,
        _vec3(38.f, 1.f, 10.f),
        _vec3(5.f, 5.f, 4.5f)
    );

    // MONSTER2 -> TARO
    //{
    //    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    //    NULL_CHECK(gameLayer);

    //    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    //    _vec3 roomBasePos = m_RoomData[(int)Room::MONSTER2].terrainOffset;
    //    _vec3 triggerPos = roomBasePos + _vec3(18.f, 1.f, 26.f);

    //    trigger->Set_Bounds(triggerPos, _vec3(4.5f, 5.f, 5.f));
    //    trigger->Set_TriggerOnce(true);

    //    trigger->Set_Callback([this]()
    //        {
    //            m_RequestChangeScene = true;
    //            m_NextSceneType = SCENETYPE::SC_LOGO_BOSS1;
    //        });

    //    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);

    //}

    /*Create_Trigger_Between(
        Room::MONSTER2,
        Room::TARO,
        _vec3(18.f, 1.f, 26.f),     
        _vec3(4.5f, 5.f, 5.f)      
    );*/

    // MONSTER2 -> BOSS
    //Create_Trigger_Between(
    //    Room::MONSTER2,
    //    Room::BOSS,
    //    _vec3(18.f, 1.f, 0.f),
    //    _vec3(5.f, 5.f, 4.5f)
    //);
}

void CStage_Dungeon::Create_Trigger_Between(Room fromRoom, Room toRoom, _vec3 offsetFromRoom, _vec3 size)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    _vec3 roomBasePos = m_RoomData[(_int)fromRoom].terrainOffset;
    _vec3 triggerPos = roomBasePos + offsetFromRoom;

    trigger->Set_Bounds(triggerPos, size);
    trigger->Set_TriggerOnce(true);

    trigger->Set_Callback([this, toRoom]()
        {
            Change_Room(toRoom);
        });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage_Dungeon::Create_Trigger_Between(Room fromRoom, _vec3 offsetFromRoom, _vec3 size,
    std::function<void()> callback)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    _vec3 roomBasePos = m_RoomData[(int)fromRoom].terrainOffset;
    _vec3 triggerPos = roomBasePos + offsetFromRoom;

    trigger->Set_Bounds(triggerPos, size);
    trigger->Set_TriggerOnce(true);
    trigger->Set_Callback(callback);

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage_Dungeon::Change_Room(Room room)
{
    Room nextRoom = room;

    // 이전 방 정리
    UnLoad_Room(m_CurrentRoom);

    m_CurrentRoom = nextRoom;

    Load_Room(nextRoom);

    // 플레이어 위치 이동
    if (m_PlayerTransform)
    {
        m_PlayerTransform->Set_Pos(m_RoomData[static_cast<_int>(nextRoom)].spawnPos);
    }

}

HRESULT CStage_Dungeon::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    for (_int i = 0; i < (_int)Room::DUNGEON_END; i++)
    {
        Room room = static_cast<Room>(i);

        m_Terrains[room] = CTerrain::Create(m_GraphicDev);
        NULL_CHECK_RETURN(m_Terrains[room], E_FAIL);

        layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrains[room]);
    }

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
            &m_Player),
        L"Persistent object setup failed");

    NULL_CHECK_RETURN(m_Player, E_FAIL);

    // 플레이어 Transform
    m_PlayerTransform = dynamic_cast<CTransform*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    NULL_CHECK_RETURN(m_PlayerTransform, E_FAIL);

    m_PlayerInventoryCom =
        dynamic_cast<CInventory*>(m_Player->Get_Component(
            ID_STATIC,
            COMPONENTTYPE::INVENTORY));

    NULL_CHECK_RETURN(m_PlayerInventoryCom, E_FAIL);

    m_PlayerCombatStatCom =
        dynamic_cast<CCombatStat*>(m_Player->Get_Component(
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

    CTransform* humanTr = gameObject->Get_Transform();
    if (humanTr)
        humanTr->Set_Pos(_vec3(21.f, 1.f, 10.f));

    {
        // HumanMonster
        gameObject = CHumanMonster::Create(m_GraphicDev);
        NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CHumanMonster::Create() returned null")

            FAILED_CHECK_MSG(
                layer->Add_GameObject(OBJTYPE::HUMANMONSTER, gameObject),
                L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

        CTransform* humanTr = gameObject->Get_Transform();
        if (humanTr)
            humanTr->Set_Pos(_vec3(26.f, 1.f, 10.f));
    }

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
            .SetTarget(m_PlayerTransform)
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
