#include "pch.h"
#include "CStage.h"

#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CCombatStat.h"
#include "CConstructionMgr.h"
#include "CCookingController.h"
#include "CDInputMgr.h"
#include "CDroppedItem.h"
#include "CDropSystem.h"
#include "CFloaterMonster.h"
#include "CHarvestableHerb.h"
#include "CHumanMonster.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CPlayerSkill.h"
#include "CRock.h"
#include "CSceneMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTree.h"
#include "CTrigger_Dungeon.h"
#include "CUIBuildMenu.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIItemGetManager.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUITabMenu.h"
#include "CUIWeapon.h"
#include "CWorkBench.h"
#include "ItemInstance.h"
#include "CHarvestableHerb.h"
#include "CPlayerSkill.h"
#include "CRock.h"
#include "CTree.h"
#include "CTrigger_Dungeon.h"
#include "CUIBuildMenu.h"
#include "CUI_Dialogue.h"
#include "CKitchen.h"
#include "CUICookingGame.h"
#include "CCookingController.h"
#include "CCreateHelper.h"
#include "CUICookingMenu.h"

class CUIBuildMenu;

CStage::CStage(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_PlayerSkillCom(nullptr),
      m_Terrain(nullptr)
{
}

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
        int id           = CItemDB::GetInstance()->GetVector()[reverseIndex].id;

        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 15.f, 1.f, 10.f + (i * 3.f) });
    }

    // Construction Manager 의 Placer초기화
    CConstructionMgr::GetInstance()->Create_Placer(m_GraphicDev);

    return S_OK;
}

void CStage::Initialize_Scene()
{
    CScene::Initialize_Scene();

    // 초기 맵 불러오기 <- 이거 Initialize에서 진행해야 맵 오브젝트가 잘 불러진다.
    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Village2.map");

    // 던전 이동 트리거 만들기
    //Create_Trigger();
}

_int CStage::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    CConstructionMgr::GetInstance()->Update_ConstructionMgr(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS1);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_G))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS2);

    // Building Mode Test : Temple
    if (CDInputMgr::GetInstance()->KeyDown(DIK_B))
    {
        if (CConstructionMgr::GetInstance()->IsBuilding())
            CConstructionMgr::GetInstance()->End_BuildMode();
        else
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::TEMPLE);
    }

    // Building Mode Test : Kitchen
    if (CDInputMgr::GetInstance()->KeyDown(DIK_N))
    {
        if (CConstructionMgr::GetInstance()->IsBuilding())
            CConstructionMgr::GetInstance()->End_BuildMode();
        else
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::KITCHEN);
    }



    //// xptmxm dksdmstn
    //if(CDInputMgr::GetInstance()->KeyDown(DIK_U))
    //{
    //   CHarvestableHerb*  tmpObject = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::SMALLBERRY);
    //   NULL_CHECK_RETURN_MSG(tmpObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    //       SpawnObjectOnLayer(tmpObject, OBJTYPE::INTERACTIVE);

    //    tmpObject->Get_Transform()->Set_Pos(20.f, 0.6f, 20.f);


    //    RecipeInst r{ 4001 ,3,2001 };

    //    CUICookingGame* cook = CUICookingGame::Create(m_GraphicDev, tmpObject, r);

    //    SpawnUIOnLayer(cook, OBJTYPE::UI);

    //    cook->Initialize_GameObject();

 
  
    //}


    return exit;
}

void CStage::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage::Render_Scene()
{
    // debug 용
    CConstructionMgr::GetInstance()->Render_ConstructionMgr();
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

    m_PlayerSkillCom = dynamic_cast<CPlayerSkill*>(gameObject->Get_Component(ID_STATIC, COMPONENTTYPE::PL_SKILL));

    NULL_CHECK_RETURN(m_PlayerSkillCom, E_FAIL);

    // -----------------------------
    // Monster
    // -----------------------------

    // HumanMonster
    gameObject = CHumanMonster::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CHumanMonster::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::HUMANMONSTER, gameObject),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

    gameObject = CFloaterMonster::Create(m_GraphicDev);
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
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed"
    );

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
    ///////////////////////////////////////////
    // 인터렉티브 오브젝트
    //////////////////////////////////////////
    CHarvestableHerb* tmpObject;
    tmpObject = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::BIGBERRY);
    NULL_CHECK_RETURN_MSG(tmpObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpObject),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpObject->Get_Transform()->Set_Pos(30.f, .6f, 30.f);

    //
    tmpObject = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::SMALLBERRY);
    NULL_CHECK_RETURN_MSG(tmpObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpObject),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpObject->Get_Transform()->Set_Pos(20.f, 0.6f, 20.f);
    //
    CRock* tmpRock;
    tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::SMALLROCK);
    NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 25.f);
    //

    tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::BIGROCK);
    NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 20.f);
    //

    tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::MIDDLEROCK);
    NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 30.f);

    //
    CTree* tmpTree;

    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE1);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(20.f, 1.f, 30.f);
    //
    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE2);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 25.f);
    //
    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE3);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 20.f);
    //
    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE4);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 15.f);
    //
    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::REALSMALLTREE);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(15.f, 2.f, 30.f);
    //
    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::REALBIGTREE);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BUILDING, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(10.f, 2.5f, 30.f);
    //
    auto workBench = CWorkBench::Create(m_GraphicDev);
    m_WorkBench = workBench;
    
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BUILDING, workBench),
        L"Add_GameObject(CWorkBench) failed");
    //
    auto kitchen = CKitchen::Create(m_GraphicDev);
    m_Kitchen = kitchen;
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BUILDING, kitchen),
        L"Add_GameObject(CWorkBench) failed");


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

    //CUITabMenu
    CUITabMenu* uiTabMenu = CUITabMenu::Create(m_GraphicDev);
    uiTabMenu->SetInventory(m_PlayerInventoryCom);
    uiTabMenu->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    uiTabMenu->Initialize();

    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiTabMenu),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UITabMenu) failed");

    FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGetManager>
                     (OBJTYPE::UI, L"UI_ItemGetManager",
                         m_GraphicDev, layer,
                         &gameObject),
                     L"Persistent object setup failed: UI_ItemGetManager")
    dynamic_cast<CUIItemGetManager*>(gameObject)->SetInventory(m_PlayerInventoryCom);
    dynamic_cast<CUIItemGetManager*>(gameObject)->Initialize();

    CUIBuildMenu* uiBuildMenu = CUIBuildMenu::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiBuildMenu),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UI_BuildMenu) failed");
    m_WorkBench->Set_BuildMenu(uiBuildMenu);
    CCookingController* cookingController = dynamic_cast<CCookingController*>(m_WorkBench->Get_Component(COMPONENTID::ID_STATIC, COMPONENTTYPE::COOKCTRL));
    uiBuildMenu->Set_Inventory(m_PlayerInventoryCom);
    uiBuildMenu->Set_CookingController(cookingController);
    /*uiBuildMenu->SetCellEvent([]()
    {
        return dynamic_cast<CCookingController*>(m_WorkBench->Get_Component(COMPONENTID::ID_STATIC,COMPONENTTYPE::COOKCTRL))->CanCooK(uiBuildMenu->,m_PlayerInventoryCom)})*/

    CUICookingMenu* uiCookingMenu = CUICookingMenu::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiCookingMenu),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UI_CookingMenu) failed");
    m_Kitchen->Set_CookingMenu(uiCookingMenu);
    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage::Ready_Prototype()
{
    return S_OK;
}

void CStage::Create_Trigger()
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    // Terrain 위치에 맞춰서 스폰
    _vec3 triggerPos = m_Terrain->Get_Transform()->Get_Pos();
    _vec3 offset     = _vec3(18.f, 0.f, 46.f);

    // 트리거 위치, 크기 세팅
    trigger->Set_Bounds(triggerPos + offset, _vec3(10.f, 5.f, 5.f));
    trigger->Set_TriggerOnce(false);

    trigger->Set_Callback([this]
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(
            SCENETYPE::SC_DUNGEON);
    });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
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
    CDropSystem::GetInstance()->Clear_Item();
    CDroppedItem::Reset_WeaponExplainUI();
    CConstructionMgr::GetInstance()->Release_Placer();

    Engine::CScene::Free();
}
