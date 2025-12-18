#include "pch.h"
#include "CAnimator.h"
#include "CStage_DungeonEntrance.h"
#include "CRigidbody.h"
#include "CCamSetupBuilder.h"
#include "CGameObject.h"
#include "CHumanMonster.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSceneMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUI_Dialogue.h"

CStage_DungeonEntrance::CStage_DungeonEntrance(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
    m_Terrain(nullptr)
{
}

CStage_DungeonEntrance::~CStage_DungeonEntrance()
{
}

HRESULT CStage_DungeonEntrance::Ready_Scene()
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

void CStage_DungeonEntrance::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Dungeon_Entrance_Final.map");

    auto player = dynamic_cast<CPlayer*>(m_PlayerTransform->Get_Owner());
    NULL_CHECK(player);

    player->Set_PrologueMode(true);

    Create_Trigger();
}

_int CStage_DungeonEntrance::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CStage_DungeonEntrance::LateUpdate_Scene(const _float& timeDelta)
{
    CScene::LateUpdate_Scene(timeDelta);
}

void CStage_DungeonEntrance::Render_Scene()
{
    if (ImGui::CollapsingHeader("Light Settings"))
    {
        bool changed = false;

        changed |= ImGui::ColorEdit4("Diffuse", (_float*)&m_LightDiffuse);
        changed |= ImGui::ColorEdit4("Ambient", (_float*)&m_LightAmbient);
        changed |= ImGui::ColorEdit4("Specular", (_float*)&m_LightSpecular);
        changed |= ImGui::DragFloat3("Direction", (_float*)&m_LightDir, 0.01f);

        if (changed)
            Ready_Light();
    }

}

HRESULT CStage_DungeonEntrance::Ready_Environment_Layer(LAYERTYPE layerType)
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

HRESULT CStage_DungeonEntrance::Ready_GameLogic_Layer(LAYERTYPE layerType)
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

    m_PlayerTransform->Set_Pos(_vec3(15.f, 1.f, 0.f));


    // Monster
    gameObject = CHumanMonster::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

    gameObject->Get_Transform()->Set_Pos(28.151f, 1.f, -2.208f);
    dynamic_cast<CHumanMonster*>(gameObject)->Set_PrayMode(true);

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

HRESULT CStage_DungeonEntrance::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_DungeonEntrance::Ready_Prototype()
{
    return S_OK;
}

HRESULT CStage_DungeonEntrance::Ready_Light()
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

    // 포인트 라이트
    D3DLIGHT9 pointLight;
    ZeroMemory(&pointLight, sizeof(D3DLIGHT9));

    pointLight.Type = D3DLIGHT_POINT;
    pointLight.Position = D3DXVECTOR3(15.f, 3.f, 44.f);  // 트리거쪽
    pointLight.Range = 8.f;
    pointLight.Diffuse = D3DXCOLOR(1.f, 0.9f, 0.7f, 1.f);
    pointLight.Attenuation0 = 1.f;
    pointLight.Attenuation1 = 0.1f;
    pointLight.Attenuation2 = 0.f;

    m_GraphicDev->SetLight(1, &pointLight);
    m_GraphicDev->LightEnable(1, TRUE);

    return S_OK;
}

void CStage_DungeonEntrance::Create_Trigger()
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    // 던전용
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 dungeonPoss = _vec3(11.f, 1.f, 30.5f);
        trigger->Set_Bounds(dungeonPoss, _vec3(5.f, 5.f, 3.f));
        trigger->Set_TriggerOnce(false);

        trigger->Set_Callback([this]
            {
                // 씬 전환. 던전용 하나, 보스방용 하나
                CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_DUNGEON);
            });

        gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);

    }

    // 보스용
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 dungeonPoss = _vec3(28.7f, 1.f, 36.1f);
        trigger->Set_Bounds(dungeonPoss, _vec3(5.f, 5.f, 3.f));
        trigger->Set_TriggerOnce(false);

        trigger->Set_Callback([this]
            {
                // 씬 전환. 던전용 하나, 보스방용 하나
                CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS1);
            });

        gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
    }

}

CStage_DungeonEntrance* CStage_DungeonEntrance::Create(DEVICE graphicDev)
{
    auto stagePrologue = new CStage_DungeonEntrance(graphicDev);

    if (FAILED(stagePrologue->Ready_Scene()))
    {
        MSG_BOX("Prologue Stage Create Failed");
        Safe_Release(stagePrologue);
        return nullptr;
    }

    return stagePrologue;
}

void CStage_DungeonEntrance::Free()
{
    CScene::Free();
}
