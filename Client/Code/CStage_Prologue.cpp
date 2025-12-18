#include "pch.h"
#include "CAnimator.h"
#include "CStage_Prologue.h"
#include "CRigidbody.h"
#include "CCamSetupBuilder.h"
#include "CExcutioner.h"
#include "CState.h"
#include "CGameObject.h"
#include "CHeket.h"
#include "CHumanMonster.h"
#include "CKallamar.h"
#include "CLayer.h"
#include "CLeshy.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSceneMgr.h"
#include "CShamura.h"
#include "CSoundMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUI_Dialogue.h"

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

    Setup_DialogueCallbacks();

    return S_OK;
}

void CStage_Prologue::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Tutorial.map");

    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Prologue.wav", 0.4f, true, 1.f);
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Bird.wav", 0.15f, false, 1.f);

    auto player = dynamic_cast<CPlayer*>(m_PlayerTransform->Get_Owner());
    NULL_CHECK(player);

    player->Set_PrologueMode(true);

    Create_Trigger();

}

_int CStage_Prologue::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (m_IsExcutionerWalking)
    {
        _vec3 curPos = m_Excutioner->Get_Transform()->Get_Pos();
        _vec3 dir = m_ExcutionerTargetPos - curPos;

        float dist = D3DXVec3Length(&dir);

        if (dist < 2.1f)
        {
            m_IsExcutionerWalking = false;

            auto animator = dynamic_cast<CAnimator*>(
                m_Excutioner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
            if (animator)
                animator->Play_Animation(L"Excutioner_Excute", ANIMSTATE::ONCE);
        }
        else
        {
            D3DXVec3Normalize(&dir, &dir);
            curPos += dir * 2.f * timeDelta;
            m_Excutioner->Get_Transform()->Set_Pos(curPos);
        }
    }

    if (m_Excutioner && !m_SceneChanged)
    {
        auto animator = dynamic_cast<CAnimator*>(
            m_Excutioner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
        if (animator &&
            animator->Get_CurKey() == L"Excutioner_Excute" &&
            animator->Get_CurFrame() >= 38)
        {
            m_SceneChanged = true;
            CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_PROLOGUE);
        }
    }
    return exit;

    return exit;
}

void CStage_Prologue::LateUpdate_Scene(const _float& timeDelta)
{
    CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Prologue::Render_Scene()
{
    if (ImGui::CollapsingHeader("Light Settings"))
    {
        bool changed = false;

        changed |= ImGui::ColorEdit4("Diffuse", (float*)&m_LightDiffuse);
        changed |= ImGui::ColorEdit4("Ambient", (float*)&m_LightAmbient);
        changed |= ImGui::ColorEdit4("Specular", (float*)&m_LightSpecular);
        changed |= ImGui::DragFloat3("Direction", (float*)&m_LightDir, 0.01f);

        if (changed)
            Ready_Light();  
    }

}

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
            &m_Player),
        L"Persistent object setup failed");

    NULL_CHECK_RETURN(m_Player, E_FAIL);

    // 플레이어의 Transform 가져오기
    m_PlayerTransform =
        dynamic_cast<CTransform*>(m_Player->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    NULL_CHECK_RETURN(m_PlayerTransform, E_FAIL);

    //m_PlayerTransform->Set_Pos(_vec3(15.f, 1.f, 5.f));
    m_PlayerTransform->Set_Pos(_vec3(15.f, 1.f, 37.f));

    // Rigid
    m_PlayerRigid =
        dynamic_cast<CRigidbody*>(m_Player->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::RIGIDBODY));

    NULL_CHECK_RETURN(m_PlayerRigid, E_FAIL);

    m_PlayerRigid->Set_VelocityLimit(_vec3(2.5f, 0.f, 2.5f));

    // Monster
    gameObject = CHumanMonster::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

    gameObject->Get_Transform()->Set_Pos(18.9f, 1.f, 41.6f);
    dynamic_cast<CHumanMonster*>(gameObject)->Set_PrayMode(true);

    m_Excutioner = CExcutioner::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, m_Excutioner);

    // NPC
    m_Leshy = CLeshy::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, m_Leshy);

    m_Heket = CHeket::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, m_Heket);

    m_Kallamar = CKallamar::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, m_Kallamar);

    m_Shamura = CShamura::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, m_Shamura);


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
            .SetTarget(m_PlayerTransform)
            .PivotOffset({ 0.f, 0.f, 0.f })
            .Distance(9.5f)
            .RotX(45.f)
            .RotY(0.f)
            .LagSpeed(10.f)
            .ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(m_CamController);
    }


    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue::Ready_Prototype()
{
    return S_OK;
}

HRESULT CStage_Prologue::Ready_Light()
{
    D3DLIGHT9 lightInfo;
    ZeroMemory(&lightInfo, sizeof(D3DLIGHT9));

    lightInfo.Type = D3DLIGHT_DIRECTIONAL;
    lightInfo.Direction = m_LightDir;
    lightInfo.Diffuse = m_LightDiffuse;
    lightInfo.Ambient = m_LightAmbient;
    lightInfo.Specular = m_LightSpecular;

    m_GraphicDev->SetLight(0, &lightInfo);
    m_GraphicDev->LightEnable(0, TRUE);

    return S_OK;
}

void CStage_Prologue::Create_Trigger()
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    // Terrain 위치에 맞춰서 스폰
    _vec3 pos = _vec3(15.f, 1.f, 45.4f);

    // 트리거 위치, 크기 세팅
    trigger->Set_Bounds(pos, _vec3(5.f, 5.f, 3.f));
    trigger->Set_TriggerOnce(false);

    trigger->Set_Callback([this]
        {
            // 대화 시작
            Start_Dialogue();
        });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage_Prologue::Setup_DialogueCallbacks()
{
    NULL_CHECK(m_Dialogue);

    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            OnDialogueEnd(id);
        });
}

void CStage_Prologue::OnDialogueEnd(const string& id)
{
    auto camMovement = m_CamController->Get_OwnerCamMovement();

    if (id == "Heket_Start")
    {
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Kallamar->Get_Transform())
            .Distance(11.4f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.f)
            .Apply();

        camMovement->Set_OnTransitionEnd([this]()
            {
                m_Dialogue->Start_Dialogue("Kalla_Start");
            });
    }

    else if (id == "Kalla_Start")
    {
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Leshy->Get_Transform())
            .Distance(11.4f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.f)
            .Apply();

        camMovement->Set_OnTransitionEnd([this]()
            {
                m_Dialogue->Start_Dialogue("Leshy_Start");
            });
    }

    else if (id == "Leshy_Start")
    {
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Shamura->Get_Transform())
            .Distance(11.4f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.f)
            .Apply();

        camMovement->Set_OnTransitionEnd([this]()
            {
                m_Dialogue->Start_Dialogue("Shamura_Start");
            });
    }

    else if (id == "Shamura_Start")
    {
        // 처형 시작
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Player->Get_Transform())
            .Distance(5.4f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.f)
            .Apply();

        m_IsExcutionerWalking = true;
        m_ExcutionerTargetPos = m_Player->Get_Transform()->Get_Pos() + _vec3(1.f, 0.f, 0.f);

        auto playerState = dynamic_cast<CState*>(
            m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

        if (playerState)
            playerState->Change_State(ACTORSTATE::KNEEL);

    }
}

void CStage_Prologue::Start_Dialogue()
{
    auto camMovement = m_CamController->Get_OwnerCamMovement();

    CCamSetupBuilder(m_CamController)
        .SetTarget(m_Heket->Get_Transform())
        .Distance(11.4f)
        .LagSpeed(3.f)
        .ZoomSpeed(2.f)
        .Apply();

    camMovement->Set_OnTransitionEnd([this]()
        {
            m_Dialogue->Start_Dialogue("Heket_Start");
        });

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
