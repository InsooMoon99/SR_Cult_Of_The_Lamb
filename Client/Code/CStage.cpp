#include "pch.h"
#include "CStage.h"

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
#include "CSoundMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIItemGet.h"
#include "CUIItemGetManager.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUITabMenu.h"
#include "CUIWeapon.h"
#include "ItemInstance.h"

CStage::CStage(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_Terrain(nullptr)
{ }

CStage::~CStage()
{ }

HRESULT CStage::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    // 안은수 test
    CDropSystem::GetInstance()->SetCurrentScene(this);

    for (int i = 0; i < CItemDB::GetInstance()->GetVector().size(); i++)
    {
        int          id   = CItemDB::GetInstance()->GetVector()[i].id;
        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 5.f, 1.f, 10.f + (i * 3.f) });
    }

    int size = CItemDB::GetInstance()->GetVector().size();
    for (int i = 0; i < size; i++)
    {
        int reverseIndex = size - 1 - i;
        int id           = CItemDB::GetInstance()->GetVector()[reverseIndex].id;

        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 10.f, 1.f, 10.f + (i * 3.f) });
    }


    size = CItemDB::GetInstance()->GetVector().size();
    for (int i = 0; i < size; i++)
    {
        int reverseIndex = size - 1 - i;
        int id = CItemDB::GetInstance()->GetVector()[reverseIndex].id;

        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 15.f, 1.f, 10.f + (i * 3.f) });
    }
    // BGM Test
    //CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Intro.wav", 0.5f);

    return S_OK;
}

void CStage::Initialize_Scene()
{
    CScene::Initialize_Scene();

    // 초기 맵 불러오기 <- 이거 꼭 Initialize에서 진행해야 맵 오브젝트가 잘 불러진다.
    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Default.map");
}

_int CStage::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F))
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS);
    }

    return exit;
}

void CStage::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage::Render_Scene()
{
    // debug 용
}

HRESULT CStage::Ready_Environment_Layer(LAYERTYPE layerType)
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

HRESULT CStage::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer,
        E_FAIL,
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Create() returned null");

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
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CHumanMonster::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::HUMANMONSTER, gameObject),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

    // -----------------------------
    // Boss
    // -----------------------------
    /*gameObject = CBossPhase1::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase1::Create() returned null")
    
        FAILED_CHECK_MSG(
            layer->Add_GameObject(OBJTYPE::BOSS1, gameObject),
            L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");*/

    // -----------------------------
    // Camera
    // -----------------------------

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA,
            L"MainCamera",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");

    // 카메라 타겟을 플레이어로 설정
    auto mainCam = dynamic_cast<CMainCamera*>(gameObject);
    if (mainCam)
    {
        mainCam->Set_CamTarget(playerTransform);

        auto camCom = dynamic_cast<CCameraCom*>(
            mainCam->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAMERA));

        if (camCom)
            CMainEditorMgr::GetInstance()->Set_MainCamera(camCom); // 에디터에 카메라 설정
    }

    m_Layers.insert({ layerType, layer });




    return S_OK;
}

HRESULT CStage::Ready_UI_Layer(LAYERTYPE layerType)
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
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    // FAILED_CHECK_MSG(
    //     Engine::AcquirePersistentObject<CUISkillCount>(
    //         OBJTYPE::UI,
    //         L"UI_SkillCount",
    //         m_GraphicDev,
    //         layer,
    //         &gameObject),
    //     L"Persistent object setup failed: UI_SkillCount");

    //CUITabMenu
    CUITabMenu* uiTabMenu = CUITabMenu::Create(m_GraphicDev);
    uiTabMenu->SetInventory(m_PlayerInventoryCom);
    uiTabMenu->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    uiTabMenu->Initialize();

    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiTabMenu),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UITabMenu) failed");

   // FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGet>
   //     (OBJTYPE::UI, L"UI_ItemGet",
   //         m_GraphicDev, layer,
   //         &gameObject), L"Persistent object setup failed: UI_ItemGet")
   //
    FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGetManager>
        (OBJTYPE::UI, L"UI_ItemGetManager",
            m_GraphicDev, layer,
            &gameObject), L"Persistent object setup failed: UI_ItemGetManager")
    dynamic_cast<CUIItemGetManager*>(gameObject)->SetInventory(m_PlayerInventoryCom);
    dynamic_cast<CUIItemGetManager*>(gameObject)->Initialize();

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage::Ready_Prototype()
{
    return S_OK;
}

CStage* CStage::Create(DEVICE graphicDev)
{
    auto stage = new CStage(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage::Free()
{
    Engine::CScene::Free();
}
