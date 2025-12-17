#include "pch.h"
#include "CStage_Prologue.h"

#include "CCamSetupBuilder.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSoundMgr.h"
#include "CTerrain.h"
#include "CTransform.h"

CStage_Prologue::CStage_Prologue(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Terrain(nullptr)
{ }

CStage_Prologue::~CStage_Prologue()
{ }

HRESULT CStage_Prologue::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    return S_OK;
}

void CStage_Prologue::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Tutorial.map");

    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Prologue.wav", 0.4f, true, 1.f);
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Bird.wav", 0.15f, false, 1.f);
}

_int CStage_Prologue::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CStage_Prologue::LateUpdate_Scene(const _float& timeDelta)
{
    CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Prologue::Render_Scene()
{ }

HRESULT CStage_Prologue::Ready_Environment_Layer(LAYERTYPE layerType)
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

HRESULT CStage_Prologue::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL);

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
            .PivotOffset({ 0.f, 0.f, 0.f })
            .Distance(12.f)
            .RotX(45.f)
            .RotY(0.f)
            .LagSpeed(10.f)
            .ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(camController);
    }

    //
    //TODO 여기에 오브젝트 추가
    //

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    //
    //TODO 여기에 UI 추가
    //

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue::Ready_Prototype()
{
    return S_OK;
}

HRESULT CStage_Prologue::Ready_Light()
{
    //CScene::Ready_Light();
    return S_OK;
}

CStage_Prologue* CStage_Prologue::Create(DEVICE graphicDev)
{
    auto stagePrologue = new CStage_Prologue(graphicDev);

    if (FAILED(stagePrologue->Ready_Scene()))
    {
        MSG_BOX("Prologue Stage Create Failed");
        Safe_Release(stagePrologue);
        return nullptr;
    }

    return stagePrologue;
}

void CStage_Prologue::Free()
{
    CScene::Free();
}
