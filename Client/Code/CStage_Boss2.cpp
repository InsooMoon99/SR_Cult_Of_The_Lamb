#include "pch.h"
#include "CStage_Boss2.h"
#include "CBossPhase1.h"
#include "CBossPhase2.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CMainEditorMgr.h"
#include "CMainMenu.h"
#include "CWall.h"
#include "CDropSystem.h"
#include "CBoss_Slave_Test.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"

CStage_Boss2::CStage_Boss2(DEVICE graphicDev)
    : Engine::CScene(graphicDev), m_Terrain(nullptr)
{

}

CStage_Boss2::~CStage_Boss2()
{ }

HRESULT CStage_Boss2::Ready_Scene()
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

void CStage_Boss2::Initialize_Scene()
{
    CScene::Initialize_Scene();

    if (m_Terrain)
        m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Boss2.map");


}   

_int CStage_Boss2::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CStage_Boss2::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Boss2::Render_Scene()
{
    // debug 용
}

HRESULT CStage_Boss2::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    
    // Wall
    CWall* wall = CWall::Create(m_GraphicDev);
    NULL_CHECK_RETURN(wall, E_FAIL);

    layer->Add_GameObject(OBJTYPE::WALL, wall);

    m_Terrain = CTerrain::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_Terrain, E_FAIL);

    layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrain);

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CStage_Boss2::Ready_GameLogic_Layer(LAYERTYPE layerType)
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
            OBJTYPE::PLAYER, L"Player", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed");

    const auto playerTransform = dynamic_cast<CTransform*>(gameObject->Get_Component(ID_DYNAMIC,COMPONENTTYPE::TRANSFORM));
    playerTransform->Set_Pos(_vec3(67.f, 1.f, 32.3f));

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
            .Distance(20.f)
            .RotX(33.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(camController);
    }

    // 춘식이 등장
    gameObject = CBoss_SlaveTest::Create(m_GraphicDev);
    NULL_CHECK_RETURN(gameObject, E_FAIL);

    const auto chunSikTrans = gameObject->Get_Transform();
    NULL_CHECK_RETURN(chunSikTrans, E_FAIL);

    chunSikTrans->Set_Pos(_vec3(27.f, 3.1f, 18.f));

    layer->Add_GameObject(OBJTYPE::NPC, gameObject);

    // 보스 소환
    gameObject = CBossPhase2::Create(m_GraphicDev);
    NULL_CHECK_RETURN(gameObject, E_FAIL);

    layer->Add_GameObject(OBJTYPE::BOSS2, gameObject);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss2::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

CStage_Boss2* CStage_Boss2::Create(DEVICE graphicDev)
{
    auto stage = new CStage_Boss2(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Boss2 Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_Boss2::Free()
{
    Engine::CScene::Free();
}
