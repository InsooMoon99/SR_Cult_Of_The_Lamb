#include "pch.h"
#include "CCombatStat.h"
#include "CAnimator.h"
#include "CStage_Prologue2.h"
#include "CRigidbody.h"
#include "CCamSetupBuilder.h"
#include "CWeaponEquip.h"
#include "CState.h"
#include "CGameObject.h"
#include "CHumanMonster.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSoundMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUI_Dialogue.h"
#include "CUI_MonsterHp.h"

CStage_Prologue2::CStage_Prologue2(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Terrain(nullptr)
{ }

CStage_Prologue2::~CStage_Prologue2()
{ }

HRESULT CStage_Prologue2::Ready_Scene()
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

void CStage_Prologue2::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Tutorial2.map");

    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Prologue.wav", 0.4f, true, 1.f);
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Bird.wav", 0.15f, false, 1.f);

    auto player = dynamic_cast<CPlayer*>(m_PlayerTransform->Get_Owner());
    NULL_CHECK(player);

    //player->Set_PrologueMode(true);

    auto playerAnimator = dynamic_cast<CAnimator*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    auto playerState = dynamic_cast<CState*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    if (playerAnimator)
        playerAnimator->Play_Animation(L"Prologue_Rebirth", ANIMSTATE::ONCE);

    if (playerState)
        playerState->Change_State(ACTORSTATE::PLAYER_REBIRTH);

    m_IsRebirthPlaying = true;  

    m_PlayerRigid->Set_VelocityLimit(_vec3(10.f, 100.f, 10.f));

    // Hp Bar 생성 -> 얘네는 에디터에서 생성을 안하고, 처음에 배치를 해주는 애들이기 때문에
    // 체력 UI를 새로 생성해주어야만 한다.
    auto uiLayer = m_Layers[LAYERTYPE::UI];
    for (auto monster : m_Monsters)
    {
        auto combat = dynamic_cast<CCombatStat*>(
            monster->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        if (combat)
        {
            CUI_MonsterHp* hpBar = CUI_MonsterHp::Create(m_GraphicDev, combat);
            uiLayer->Add_GameObject(OBJTYPE::UI, hpBar);
            hpBar->Initialize_GameObject();
        }
    }

}

_int CStage_Prologue2::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    Rebirth_End();

    return exit;
}

void CStage_Prologue2::LateUpdate_Scene(const _float& timeDelta)
{
    CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Prologue2::Render_Scene()
{ }

HRESULT CStage_Prologue2::Ready_Environment_Layer(LAYERTYPE layerType)
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

HRESULT CStage_Prologue2::Ready_GameLogic_Layer(LAYERTYPE layerType)
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
    m_PlayerTransform->Set_Pos(_vec3(15.f, 1.f, 44.f));

    // Rigid
    m_PlayerRigid =
        dynamic_cast<CRigidbody*>(m_Player->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::RIGIDBODY));

    NULL_CHECK_RETURN(m_PlayerRigid, E_FAIL);

    // Monster1
    {
        gameObject = CHumanMonster::Create(m_GraphicDev);
        layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

        gameObject->Get_Transform()->Set_Pos(18.9f, 1.f, 41.6f);
        dynamic_cast<CHumanMonster*>(gameObject)->Set_Battle(false);
        m_Monsters.push_back(gameObject);
    }
    
    // Monster2
    {
        gameObject = CHumanMonster::Create(m_GraphicDev);
        layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

        gameObject->Get_Transform()->Set_Pos(13.9f, 1.f, 41.6f);
        dynamic_cast<CHumanMonster*>(gameObject)->Set_Battle(false);
        m_Monsters.push_back(gameObject);
    }


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
            .Distance(12.f)
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

HRESULT CStage_Prologue2::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue2::Ready_Prototype()
{
    return S_OK;
}

HRESULT CStage_Prologue2::Ready_Light()
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

void CStage_Prologue2::Create_Trigger()
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

void CStage_Prologue2::Setup_DialogueCallbacks()
{
    NULL_CHECK(m_Dialogue);

    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            OnDialogueEnd(id);
        });
}

void CStage_Prologue2::OnDialogueEnd(const string& id)
{
    
}

void CStage_Prologue2::Start_Dialogue()
{
    

}

void CStage_Prologue2::Rebirth_End()
{
    if (m_IsRebirthPlaying)
    {
        auto playerAnimator = dynamic_cast<CAnimator*>(
            m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

        if (playerAnimator && playerAnimator->Is_AnimationFinished())
        {
            m_IsRebirthPlaying = false;

            auto player = dynamic_cast<CPlayer*>(m_Player);
            if (player)
                player->Set_PrologueMode(false);

            auto weaponEquip = dynamic_cast<CWeaponEquip*>(
                m_Player->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP));

            if (weaponEquip)
            {
                ItemInstance swordItem;
                swordItem.itemId = 3001;

                weaponEquip->Equip_Weapon(swordItem);
            }

            auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
            if (gameLayer)
            {
                auto& objects = gameLayer->Get_Objects();
                auto iter = objects.find(OBJTYPE::MONSTER);
                if (iter != objects.end())
                {
                    for (auto monster : iter->second)
                    {
                        auto humanMonster = dynamic_cast<CHumanMonster*>(monster);
                        if (humanMonster)
                            humanMonster->Enable_Battle();
                    }
                }
            }
        }
    }
}

CStage_Prologue2* CStage_Prologue2::Create(DEVICE graphicDev)
{
    auto stagePrologue = new CStage_Prologue2(graphicDev);

    if (FAILED(stagePrologue->Ready_Scene()))
    {
        MSG_BOX("Prologue Stage Create Failed");
        Safe_Release(stagePrologue);
        return nullptr;
    }

    return stagePrologue;
}

void CStage_Prologue2::Free()
{
    CScene::Free();
}
