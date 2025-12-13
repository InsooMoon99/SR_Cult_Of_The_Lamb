#include "pch.h"
#include "CStage_Boss2.h"
#include "CBossPhase1.h"
#include "CParticleHelper.h"
#include "CBossPhase2.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CCombatStat.h"
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
#include "CUI_BossHp.h"
#include "CLightMgr.h"

CStage_Boss2::CStage_Boss2(DEVICE graphicDev)
    : Engine::CScene(graphicDev), m_Terrain(nullptr), m_Boss(nullptr)
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

    if (FAILED(Ready_Light()))
        return E_FAIL;

    // 아이템 드롭을 위한 씬 세팅 Eyeball 죽일 때마다 체력 아이템 떨구기 ?
    CDropSystem::GetInstance()->SetCurrentScene(this);

    return S_OK;
}

void CStage_Boss2::Initialize_Scene()
{
    CScene::Initialize_Scene();

    if (m_Terrain)
        m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Boss1.map");

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

    m_Layers.insert({ layerType, layer });          

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

    // 플레이어 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CPlayer>(
            OBJTYPE::PLAYER, L"Player", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed");

    const auto playerTransform = dynamic_cast<CTransform*>(gameObject->Get_Component(ID_DYNAMIC,COMPONENTTYPE::TRANSFORM));
    playerTransform->Set_Pos(_vec3(48.f, 1.f, 38.3f));

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

    chunSikTrans->Set_Pos(_vec3(37.f, 3.1f, 38.f));

    layer->Add_GameObject(OBJTYPE::NPC, gameObject);

    // 보스 소환
    m_Boss = CBossPhase2::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_Boss, E_FAIL);

    m_Boss->Get_Transform()->Set_Pos(_vec3(48.f, 5.7f, 58.3f));

    layer->Add_GameObject(OBJTYPE::BOSS2, m_Boss);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss2::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* gameObject = nullptr;

    auto combat = dynamic_cast<CCombatStat*>(m_Boss->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    NULL_CHECK_RETURN(combat, E_FAIL);

    gameObject = CUI_BossHp::Create(m_GraphicDev, combat);
    layer->Add_GameObject(OBJTYPE::UI, gameObject);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss2::Ready_Light()
{
    D3DLIGHT9 lightInfo;
    ZeroMemory(&lightInfo, sizeof(D3DLIGHT9));

    //lightInfo.Type = D3DLIGHT_DIRECTIONAL;  // 방향성 조명
    lightInfo.Type = D3DLIGHT_POINT;          // Point Light

    lightInfo.Diffuse = m_LightDiffuse;
    lightInfo.Ambient = m_LightAmbient;   // 그림자 영역
    lightInfo.Specular = m_LightSpecular;

    // 빛의 위치 (보스맵 가운데, 위쪽으로 설정)
    lightInfo.Position = m_LightPos;

    // 빛의 범위 (설정한 범위 밖이면 완전히 어둡게?)
    lightInfo.Range = m_LightRange;

    // 감쇠 (거리에 따라 어두워지도록 해보기)
    lightInfo.Attenuation0 = m_Attenuation0;       // 상수 (0 = 기본 밝기 100%)
    lightInfo.Attenuation1 = m_Attenuation1;       // 거리 비례 (클수록 빨리 어두워진다)
    lightInfo.Attenuation2 = m_Attenuation2;       // 거리^2 비례

    if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_GraphicDev, &lightInfo, 0)))
        return E_FAIL;

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
