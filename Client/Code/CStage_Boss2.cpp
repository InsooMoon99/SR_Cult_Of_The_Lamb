#include "pch.h"
#include "CStage_Boss2.h"
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
#include "CUI_Dialogue.h"

CStage_Boss2::CStage_Boss2(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
    m_Terrain(nullptr),
    m_Boss(nullptr),
    m_Player(nullptr),
    m_CamController(nullptr),
    m_PlayerTransformCom(nullptr)
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

    Setup_BossCallbacks();

    // TODO : 일단은 바로 시작 -> 대사끝나면 시작하도록 변경 예정
    Setup_DynamicCam();
    Setup_DialogueCallbacks();

    return S_OK;
}

void CStage_Boss2::Initialize_Scene()
{
    CScene::Initialize_Scene();

    if (m_Terrain)
        m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Boss2.map");

    if (m_CamController)
    {
        auto camMovement = m_CamController->Get_OwnerCamMovement();
        camMovement->Disable_DynamicDistance();
    }
  
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
            OBJTYPE::PLAYER, L"Player", m_GraphicDev, layer, &m_Player), L"Persistent object setup failed");

    m_PlayerTransformCom = m_Player->Get_Transform();
    m_PlayerTransformCom->Set_Pos(_vec3(48.f, 1.f, 38.3f));

    // 카메라 생성
    Engine::CGameObject* cameraObject = nullptr;

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &cameraObject),
        L"Persistent object setup failed"
    );

    m_CamController = dynamic_cast<CCameraController*>(
        cameraObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (m_CamController != nullptr)
    {
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_PlayerTransformCom)
            //.PivotOffset({ 0.f, 0.f, 0.f })
            .Distance(22.f)
            .RotX(44.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(m_CamController);
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

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

void CStage_Boss2::Setup_DynamicCam()
{
    if (!m_CamController || !m_Boss)
        return;

    auto camMovement = m_CamController->Get_OwnerCamMovement();
    camMovement->Set_SecondTarget(m_Boss->Get_Transform());

    // 이거 수치 조절해야함
    camMovement->Set_DynamicDistanceParams(16.f, 23.f, 7.f, 15.f);
    camMovement->Enable_DynamicDistance(true);
}

void CStage_Boss2::Setup_BossCallbacks()
{
    auto boss = dynamic_cast<CBossPhase2*>(m_Boss);
    if (!boss) return;

    // 소리지르기
    boss->Set_OnSummonRoarStart([this]()
        {
            Start_CameraMoving(CAM_MOVE_TYPE::START_BOSS_ZOOM);
        });

    // Roar 끝
    boss->Set_OnSummonRoarEnd([this]()
        {
            m_Dialogue->Start_Dialogue("Boss2_Start", _vec2(0.f, 200.f));
        });

    boss->Set_OnSummonAnimStart([this]()
        {
            Start_CameraMoving(CAM_MOVE_TYPE::SUMMON_ZOOM_OUT);
        });

    boss->Set_OnSummonAnimEnd([this]()
        {
            Start_CameraMoving(CAM_MOVE_TYPE::EYEBALL_ZOOM);
        });

    // 전투 시작하면 Dynamic Cam 세팅, 이후 죽으면 Disable
    boss->Set_OnBattleStart([this]()
        {
            Setup_DynamicCam();
        });

    // 보스 죽을 때
    boss->Set_OnDead([this]()
        {
            m_CamController->Get_OwnerCamMovement()->Disable_DynamicDistance();
        });

    // 보스 죽고 NPC로 변신
    boss->Set_OnFollowerStart([this]()
        {

            m_Dialogue->Start_Dialogue("Boss2_Dead", _vec2(0.f, 200.f));
        });
    
}

void CStage_Boss2::Start_CameraMoving(CAM_MOVE_TYPE type)
{
    switch (type)
    {
    case CAM_MOVE_TYPE::START_BOSS_ZOOM:
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Boss->Get_Transform())
            .Distance(20.5f)
            .RotX(30.f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.5f)
            .Apply();
        break;

    case CAM_MOVE_TYPE::SUMMON_ZOOM_OUT:
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Boss->Get_Transform())
            .Distance(44.f)
            .RotX(28.f)
            .LagSpeed(4.f)
            .ZoomSpeed(3.5f)
            .Apply();
        break;

    case CAM_MOVE_TYPE::EYEBALL_ZOOM:
        // Eyeball로 줌인되고,
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Boss->Get_Transform())
            .Distance(20.5f)
            .RotX(35.f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.5f)
            .Apply();


        // 줌인 끝나면 플레이어한테 카메라 세팅
        m_CamController->Get_OwnerCamMovement()->Set_OnTransitionEnd([this]()
            {
                CCamSetupBuilder(m_CamController)
                    .SetTarget(m_PlayerTransformCom)
                    .Distance(22.f)
                    .RotX(44.f)
                    //.Snap()
                    .LagSpeed(4.f)
                    .ZoomSpeed(3.5f)
                    .Apply();

                // 이후 끝나면 Dynamic Camera 키기
                m_CamController->Get_OwnerCamMovement()->Set_OnTransitionEnd([this]()
                    {
                        m_CamController->Get_OwnerCamMovement()->Enable_DynamicDistance(true);
                    });
            });
        break;
    }

    // Camera Shake
    Start_CameraShaking(type);
}

void CStage_Boss2::Start_CameraShaking(CAM_MOVE_TYPE type)
{
    auto camMovement = m_CamController->Get_OwnerCamMovement();
    NULL_CHECK(camMovement);

    switch (type)
    {
    case CAM_MOVE_TYPE::START_BOSS_ZOOM:
        camMovement->Start_CamShake(0.36f, 0.15f, 49.1f, 25.f);
        break;

    case CAM_MOVE_TYPE::SUMMON_ZOOM_OUT:
        // NONE
        break;

    case CAM_MOVE_TYPE::EYEBALL_ZOOM:
        // NONE
        break;

    }
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

void CStage_Boss2::Setup_DialogueCallbacks()
{
    NULL_CHECK(m_Dialogue);

    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            OnDialogueEnd(id);
        });
}

void CStage_Boss2::OnDialogueEnd(const string& id)
{
    if (id == "Boss2_Start")
    {
        auto boss = dynamic_cast<CBossPhase2*>(m_Boss);
        boss->Resume_Summon();
    }

    else if (id == "Boss2_Dead")
    {
        OnBoss2DeadEnd();
    }
    else if (id == "Boss2_Ending")
    {
        
    }
}

void CStage_Boss2::OnBoss2DeadEnd()
{
    // 메인메뉴 or 엔딩 씬 있으면 추가 ㄱㄱ
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
