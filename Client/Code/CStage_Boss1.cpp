#include "pch.h"
#include "CStage_Boss1.h"
#include "CBossPhase1.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CCombatStat.h"
#include "CMainMenu.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUI_BossHp.h"
#include "CUI_Dialogue.h"
#include "CWall.h"

CStage_Boss1::CStage_Boss1(DEVICE graphicDev)
    : Engine::CScene(graphicDev), m_Terrain(nullptr), m_Boss(nullptr), m_Dialogue(nullptr)
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

void CStage_Boss1::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Boss1.map");

    // Dialogue
    NULL_CHECK(m_Dialogue);

    // 대화 종료 시 콜백
    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            OnDialogueEnd(id);
        });

    m_Dialogue->Start_Dialogue("boss1_intro");
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
    m_Terrain = CTerrain::Create(m_GraphicDev);
    
    NULL_CHECK_RETURN_MSG(
        m_Terrain,
        E_FAIL,
        L"CStage_Boss1::Ready_GameLogic_Layer() failed: CTerrain::Create() returned null");
    
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrain),
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Tile) failed");
    
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
            L"Player",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");

    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    playerTransform->Set_Pos({ 47.f,1.f,10.f });
   

    // -----------------------------
    // Camera
    // -----------------------------

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed"
    );

    m_CamController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (m_CamController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(m_CamController)
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
        CMainEditorMgr::GetInstance()->Set_MainCamera(m_CamController);
    }

    // -----------------------------
    // BossPhase1
    // -----------------------------

    // 보스 생성
    m_Boss = CBossPhase1::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(m_Boss, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase2::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BOSS1, m_Boss),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Boss) failed");


    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss1::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* gameObject = nullptr;

    auto combat = dynamic_cast<CCombatStat*>(m_Boss->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    NULL_CHECK_RETURN(combat, E_FAIL);

    gameObject = CUI_BossHp::Create(m_GraphicDev, combat);
    layer->Add_GameObject(OBJTYPE::UI, gameObject);

    // Dialogue
    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss1::Ready_Prototype()
{
    // Move Loading Scene

    return S_OK;
}

void CStage_Boss1::OnDialogueEnd(const string& id)
{
    if (id == "boss1_accept")
    {
        cout << "수락" << endl;
    }

    else if (id == "boss1_reject")
    {
        cout << "거절" << endl;
    }
}

void CStage_Boss1::Create_Trigger(_vec3 pos)
{
    // 보스 연출용 처음 한번만 사용할 트리거
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
    NULL_CHECK(trigger);

    trigger->Set_Bounds(pos, _vec3(5.f, 5.f, 5.f));
    trigger->Set_TriggerOnce(true); // 한 번만 실행

    trigger->Set_Callback([this]()
        {
            Begin_BossIntro();
        });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);

}

void CStage_Boss1::Begin_BossIntro()
{
    // 카메라 연출 하면서, UI를 모두 끄는 식으로 가는게 좋을거같다.
    // 그리고 연출이 끝나면 
    cout << "일단 들어옴" << endl;
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
