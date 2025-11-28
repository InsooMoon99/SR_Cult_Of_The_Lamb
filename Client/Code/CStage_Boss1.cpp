#include "pch.h"
#include "CStage_Boss1.h"

#include "CLayer.h"
#include "CMainCamera.h"
#include "CMonster.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CDInputMgr.h"
#include "CMainEditorMgr.h"
#include "CMainMenu.h"
#include "CSceneMgr.h"
#include "CWall.h"
#include "CBossPhase1.h"
#include "CDropSystem.h"

CStage_Boss1::CStage_Boss1(DEVICE graphicDev)
    : Engine::CScene(graphicDev)
{ }

CStage_Boss1::~CStage_Boss1()
{ }

HRESULT CStage_Boss1::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    //아이템 드롭 할거면 씬 셋팅 필수
    CDropSystem::GetInstance()->SetCurrentScene(this);

    return S_OK;
}

_int CStage_Boss1::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);



    return exit;
}

void CStage_Boss1::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);


}

void CStage_Boss1::Render_Scene()
{
    // debug 용
}

HRESULT CStage_Boss1::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    // -----------------------------
    // Tile
    // -----------------------------
    gameObject = CTerrain::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(
        gameObject,
        E_FAIL,
        L"CStage_Boss1::Ready_GameLogic_Layer() failed: CTerrain::Create() returned null");

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::TERRAIN, gameObject),
        L"CStage_TaroTest::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Tile) failed");

    // Wall
    CWall* wall = CWall::Create(m_GraphicDev);
    NULL_CHECK_RETURN(wall, E_FAIL);

    layer->Add_GameObject(OBJTYPE::WALL, wall);

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CStage_Boss1::Ready_GameLogic_Layer(LAYERTYPE layerType)
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
   

    // -----------------------------
    // Boss
    // -----------------------------
    //gameObject = CBossPhase1::Create(m_GraphicDev);
    //NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase1::Create() returned null")
    //
    //    FAILED_CHECK_MSG(
    //        layer->Add_GameObject(OBJTYPE::BOSS1, gameObject),
    //        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");
    
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


    // -----------------------------
    // BossPhase1
    // -----------------------------
    
    // 보스 생성
    gameObject = CBossPhase1::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase1::Create() returned null")

        FAILED_CHECK_MSG(
            layer->Add_GameObject(OBJTYPE::BOSS1, gameObject),
            L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(HUMANMONSTER) failed");

    const auto BossTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    BossTransform->Set_Pos({ 50.f,BossTransform->Get_Scale().y*0.5f,50.f });

    return S_OK;
 
}

HRESULT CStage_Boss1::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss1::Ready_Prototype()
{
    // Move Loading Scene

    return S_OK;
}

CStage_Boss1* CStage_Boss1::Create(DEVICE graphicDev)
{
    auto stage = new CStage_Boss1(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Boss1 Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_Boss1::Free()
{
    Engine::CScene::Free();
}
