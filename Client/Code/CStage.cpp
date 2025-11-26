#include "pch.h"
#include "CStage.h"

#include "CCombatStat.h"
#include "CDropSystem.h"
#include "CEditContext.h"
#include "CHumanMonster.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CTestMonster.h"
#include "CTransform.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIInventory.h"
#include "CUIInventoryCell.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUISKillCount.h"
#include "CUIWeapon.h"
#include "ItemInstance.h"
#include "CBossPhase1.h"


#include "CDInputMgr.h"
#include "CSceneMgr.h"

CStage::CStage(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr)
{}

CStage::~CStage()
{}

HRESULT CStage::Ready_Scene()
{
    // 로딩 스레드로 역할 이동
    //if (FAILED(Ready_Prototype()))
    //    return E_FAIL;

    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;
    // 안은수 test
    CDropSystem::GetInstance()->SetCurrentScene(this);

    for (int i = 0; i < CItemDB::GetInstance()->GetVector().size();i++)
    {
        int id = CItemDB::GetInstance()->GetVector()[i].id;
        ItemInstance inst = { id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 10.f, 0.f, 10.f + (i*3.f) });

    }

    return S_OK;
}

_int CStage::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    //안은수 테스트
    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_0) & 0x80)
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_MAINMENU);
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

    Engine::CGameObject* pGameObject = nullptr;

    m_Layers.insert({ layerType, layer });          // scene -> map

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
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");

    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));
    m_PlayerInventoryCom =
        dynamic_cast<CInventory*>(gameObject->Get_Component(
        ID_STATIC,
        COMPONENTTYPE::INVENTORY));

    m_PlayerCombatStatCom =
        dynamic_cast<CCombatStat*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::COMBATSTAT));

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

   
    gameObject = CBossPhase1::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase1::Create() returned null")

        FAILED_CHECK_MSG(
            layer->Add_GameObject(OBJTYPE::BOSS1, gameObject),
            L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

    // -----------------------------
    // Camera
    // -----------------------------

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA,
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

    gameObject = CUIPlayerPanel::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, gameObject),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIPlayerPanel) failed");

    CUIPlayerPanel* playerPanel = dynamic_cast<CUIPlayerPanel*>(gameObject);

    playerPanel->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    gameObject = CUIMp::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, gameObject),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIMp) failed");

    CUIMp* uiMp = dynamic_cast<CUIMp*>(gameObject);
    uiMp->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    gameObject = CUICircle::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, gameObject),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUICircle) failed");

    gameObject = CUIWeapon::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, gameObject),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIWeapon) failed");

    //왼쪽 상단 스킬 아이콘 UI
    CUIIcon* pgameObject = CUIIcon::Create(m_GraphicDev);
    pgameObject->Set_TextureIndex(1);
    pgameObject->Get_Transform()->Set_Pos(-535.f, 260.f, 0.f);
    pgameObject->Get_Transform()->Set_Scale(9.5f, 11.f, 0.f);

    FAILED_CHECK_MSG(
    layer->Add_GameObject(OBJTYPE::UI, pgameObject),
    L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    CUISkillCount* uiSkillCount = CUISkillCount::Create(m_GraphicDev);
    
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, uiSkillCount),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUISKillCount) failed");

    //CUIInventoryCell* uiInventoryCell = CUIInventoryCell::Create(m_GraphicDev);
    //
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::UI, uiInventoryCell),
    //    L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UIInventoryCell) failed");

    //Inventory UI
    CUIInventory* uiInventory = CUIInventory::Create(m_GraphicDev);
    uiInventory->SetInventory(m_PlayerInventoryCom);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiInventory),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UIInventory) failed");


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
