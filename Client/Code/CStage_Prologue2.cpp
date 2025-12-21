#include "pch.h"
#include "CCombatStat.h"
#include "CAnimator.h"
#include "CStage_Prologue2.h"
#include "CDInputMgr.h"
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
#include "CRatau.h"
#include "CSoundMgr.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUI_Dialogue.h"
#include "CUI_DungeonLootPanel.h"
#include "CUI_Fade.h"
#include "CUI_Interactive.h"
#include "CUI_MonsterHp.h"
#include "CUI_Interactive.h"
#include "CInteractiveGrass.h"
#include "CDropSystem.h"
#include "CSceneMgr.h"

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

    //!! 드롭 시스템의 오작동을 막기 위해, SetCurrentScene을 nullptr로 초기화
    CDropSystem::GetInstance()->SetCurrentScene(nullptr);

    Set_RoomData();

    //m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Tutorial2.map");

    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Prologue.wav", 0.4f, true, 1.f);

    auto player = dynamic_cast<CPlayer*>(m_PlayerTransform->Get_Owner());
    NULL_CHECK(player);

    auto playerAnimator = dynamic_cast<CAnimator*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    auto playerState = dynamic_cast<CState*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    if (playerAnimator)
        playerAnimator->Play_Animation(L"Prologue_Rebirth", ANIMSTATE::ONCE);

    if (playerState)
        playerState->Change_State(ACTORSTATE::PLAYER_REBIRTH);

    m_IsRebirthPlaying = true;  

    m_PlayerRigid->Set_VelocityLimit(_vec3(9.f, 100.f, 9.f));

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

    m_DescriptionUI = CUI_Interactive::Create(m_GraphicDev);
    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_DescriptionUI);
    m_DescriptionUI->Set_TextureIndex(5);
    m_DescriptionUI->Get_Transform()->Set_Pos(_vec3(0.f, -240.f, 1.f));
    m_DescriptionUI->Get_Transform()->Set_Scale(_vec3(220.f,35.f,1.f));

    m_Description2UI = CUI_Interactive::Create(m_GraphicDev);
    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_Description2UI);
    m_Description2UI->Set_TextureIndex(2);
    m_Description2UI->Get_Transform()->Set_Pos(_vec3(0.f, -250.f, 1.f));
    m_Description2UI->Get_Transform()->Set_Scale(_vec3(110.f, 35.f, 1.f));
    m_Description2UI->Set_IsActive(false);
}

_int CStage_Prologue2::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    Rebirth_End();
    Fade_Check(timeDelta);

    if (m_InWarpZone && !m_WarpStarted)
    {
        m_Description2UI->Set_IsActive(true);

        if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
        {
            m_WarpStarted = true;

            m_Description2UI->Set_IsActive(false);

            auto playerState = dynamic_cast<CState*>(
                m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

            if (playerState)
                playerState->Change_State(ACTORSTATE::PLAYER_WARPOUT);
        }
    }

    if (m_WarpStarted)
    {
        m_WarpTimer += timeDelta;

        if (m_WarpTimer >= 1.0f)
        {
            CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_STAGE);
        }
        
        // auto animator = dynamic_cast<CAnimator*>(
        //     m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

        // if (animator && animator->Get_CurKey() == L"Player_WarpOut"
        //     && animator->Is_AnimationFinished())
        // {
        //     //m_PrologueCleared = true;
        // }
    }

    if (m_UIDelayTime > 0.f)
    {
        m_UIDelayTime -= timeDelta;

        return 0;
    }

    if (m_UIOpenTime > 0.f)
    {
        m_UIOpenTime -= timeDelta;

        m_DescriptionUI->Set_IsActive(true);

        if (m_UIOpenTime <= 0.f)
        {
            m_DescriptionUI->Set_IsActive(false);
            m_UIOpenTime = 0.f;
        }
    }

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

    // 프롤로그 2는 방 4개만?
    for (_int i = 0; i < static_cast<_int>(Room::PROLOGUE2_END); i++)
    {
        Room room = static_cast<Room>(i);
        m_Terrains[room] = CTerrain::Create(m_GraphicDev);
        NULL_CHECK_RETURN(m_Terrains[room], E_FAIL);

        layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrains[room]);
    }

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

        gameObject->Get_Transform()->Set_Pos(17.9f, 1.f, 41.4f);
        dynamic_cast<CHumanMonster*>(gameObject)->Set_Battle(false);
        m_Monsters.push_back(gameObject);
    }
    
    // Monster2
    {
        gameObject = CHumanMonster::Create(m_GraphicDev);
        layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

        gameObject->Get_Transform()->Set_Pos(12.3f, 1.f, 41.4f);
        dynamic_cast<CHumanMonster*>(gameObject)->Set_Battle(false);
        m_Monsters.push_back(gameObject);
    }

    // Monster3
    {
        gameObject = CHumanMonster::Create(m_GraphicDev);
        layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

        gameObject->Get_Transform()->Set_Pos(12.3f, 1.f, 45.4f);
        dynamic_cast<CHumanMonster*>(gameObject)->Set_Battle(false);
        m_Monsters.push_back(gameObject);
    }

    // Monster4
    {
        gameObject = CHumanMonster::Create(m_GraphicDev);
        layer->Add_GameObject(OBJTYPE::MONSTER, gameObject);

        gameObject->Get_Transform()->Set_Pos(17.9f, 1.f, 45.4f);
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
    //풀 생성
    CInteractiveGrass* grass = CInteractiveGrass::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(grass, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CInteractiveGrass::Create() returned null")
        FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::INTERACTIVE, grass),
            L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CInteractiveGrass) failed");
    grass->Get_Transform()->Set_Pos(110.f, 1.f, 8.f);
    grass->Set_EnableHoverCheck(false);
    grass->Check_WorldState();
    
    grass = CInteractiveGrass::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(grass, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CInteractiveGrass::Create() returned null")
        FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::INTERACTIVE, grass),
            L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CInteractiveGrass) failed");
    grass->Get_Transform()->Set_Pos(122.f, 1.f, 16.f);
    grass->Set_EnableHoverCheck(false);
    grass->Check_WorldState();

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue2::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_UI_Fade = CUI_Fade::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_UI_Fade);

    //m_UI_Interact = CUI_Interactive::Create(m_GraphicDev);
    //layer->Add_GameObject(OBJTYPE::UI, m_UI_Interact);
    //m_UI_Interact->Set_IsActive(false);
    //
    //m_ClearPanel = CUI_DungeonLootPanel::Create(m_GraphicDev);
    //layer->Add_GameObject(OBJTYPE::UI, m_ClearPanel);
    //m_ClearPanel->Initialize_GameObject();

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Prologue2::Ready_Prototype()
{
    return S_OK;
}

void CStage_Prologue2::Create_RoomTriggers()
{
    // START <-> MIDDLE1
    Create_BiDirectional_Trigger(                      
        Room::START, _vec3(29.f, 1.f, 5.f),            // 1
        Room::MIDDLE1, _vec3(0.f, 1.f, 11.6f),         // 2
        _vec3(5.f, 5.f, 4.5f));

    // MIDDLE1 <-> MIDDLE2
    Create_BiDirectional_Trigger(
        Room::MIDDLE1, _vec3(34.f, 1.f, 11.6f),       // 3
        Room::MIDDLE2, _vec3(-22.f, 1.f, 4.7f),        // 4
        _vec3(5.f, 5.f, 4.5f));

    // MIDDLE2 <-> END_ROOM
    Create_BiDirectional_Trigger(
        Room::MIDDLE2, _vec3(13.f, 1.f, 21.4f),      // 5
        Room::END_ROOM, _vec3(10.f, 1.f, -3.f),        // 6
        _vec3(5.f, 5.f, 4.5f));


    // 라타우 소환 트리거
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 roomPos = m_RoomData[static_cast<_int>(Room::END_ROOM)].terrainOffset;

        trigger->Set_Bounds(roomPos + _vec3(11.f, 1.f, 5.6f), _vec3(5.f, 5.f, 2.5f));
        trigger->Set_TriggerOnce(true); 

        trigger->Set_Callback([this]()
            {
                // 라타우 소환
                Spawn_Ratau();
            });

        m_Layers[LAYERTYPE::GAMELOGIC]->Add_GameObject(OBJTYPE::TRIGGER, trigger);
    }

    // 집가는 트리거
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 roomPos = m_RoomData[static_cast<_int>(Room::END_ROOM)].terrainOffset;

        trigger->Set_Bounds(roomPos + _vec3(11.f, 1.f, 8.6f), _vec3(5.f, 5.f, 2.5f));
        trigger->Set_TriggerOnce(false);

        trigger->Set_Callback([this]()
            {
                m_InWarpZone = true;

                //if (m_UI_Interact)
                //  m_UI_Interact->Set_IsActive(true);
            });

        m_Layers[LAYERTYPE::GAMELOGIC]->Add_GameObject(OBJTYPE::TRIGGER, trigger);
    }
}

void CStage_Prologue2::Create_BiDirectional_Trigger(Room roomA, _vec3 offsetA, Room roomB, _vec3 offsetB, _vec3 size)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    // A → B 트리거
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 roomAPos = m_RoomData[static_cast<_int>(roomA)].terrainOffset;
        trigger->Set_Bounds(roomAPos + offsetA, size);
        trigger->Set_TriggerOnce(false);
        trigger->Set_Callback([this, roomB]() {
            Change_Room(roomB);
            });
        gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
    }

    // B → A 트리거
    {
        auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
        _vec3 roomBPos = m_RoomData[static_cast<_int>(roomB)].terrainOffset;
        trigger->Set_Bounds(roomBPos + offsetB, size);
        trigger->Set_TriggerOnce(false);
        trigger->Set_Callback([this, roomA]() {
            Change_Room(roomA);
            });
        gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
    }
}

void CStage_Prologue2::Change_Room(Room room)
{
    if (m_IsFading)
        return;

    if (m_UI_Fade)
    {
        m_FromRoom = m_CurrentRoom;
        m_NextRoom = room;
        m_IsFading = true;
        m_UI_Fade->Start_FadeOut(3.5f);
    }
}

void CStage_Prologue2::Fade_Check(const _float timeDelta)
{
    if (!m_IsFading || !m_UI_Fade)
        return;

    if (!m_UI_Fade->IsFading() && m_UI_Fade->Get_Alpha() >= 1.0f)
    {
        if (m_Terrains[m_CurrentRoom])
            m_Terrains[m_CurrentRoom]->Clear_SpawnedObjects();

        for (auto& pair : m_Terrains)
            pair.second->Set_Active(false);

        m_Terrains[m_NextRoom]->Set_Active(true);
        m_CurrentRoom = m_NextRoom;

        auto key = make_pair(m_FromRoom, m_NextRoom);
        _vec3 spawnPos;

        if (m_SpawnPositions.count(key) > 0)
            spawnPos = m_SpawnPositions[key];
        else
            spawnPos = m_RoomData[static_cast<_int>(m_NextRoom)].playerSpawnPos;

        if (m_PlayerTransform)
            m_PlayerTransform->Set_Pos(spawnPos);

        m_UI_Fade->Start_FadeIn(0.5f);
        m_IsFading = false;
    }
}

void CStage_Prologue2::Connect_Rooms()
{
    m_ConnectRooms[Room::START] = { Room::MIDDLE1 };
    m_ConnectRooms[Room::MIDDLE1] = { Room::START, Room::MIDDLE2 };

    m_ConnectRooms[Room::MIDDLE2] = { Room::MIDDLE1, Room::END_ROOM };
    m_ConnectRooms[Room::END_ROOM] = { Room::MIDDLE2 };
}

void CStage_Prologue2::Set_SpawnPos()
{
    // START -> MIDDLE1
    m_SpawnPositions[{Room::START, Room::MIDDLE1}] = { 104.f, 1.f, 11.6f };
    m_SpawnPositions[{Room::MIDDLE1, Room::START}] = { 24.f, 1.f, 5.f };

    // MIDDLE1 -> MIDDLE2
    m_SpawnPositions[{Room::MIDDLE1, Room::MIDDLE2}] = { 202.f, 1.f, 5.538f };
    m_SpawnPositions[{Room::MIDDLE2, Room::MIDDLE1}] = { 130.f, 1.f, 11.2f };

    // MIDDLE2 -> END_ROOM
    m_SpawnPositions[{Room::MIDDLE2, Room::END_ROOM}] = { 310.f, 1.f, 2.14f };
    m_SpawnPositions[{Room::END_ROOM, Room::MIDDLE2}] = { 220.f, 1.f, 0.f };
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
    if (id == "Ratau_Start")
    {
        auto animator = dynamic_cast<CAnimator*>(
            m_Ratau->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

        if (animator)
            animator->Play_Animation(L"Ratau_Exit", ANIMSTATE::ONCE);

        auto camMovement = m_CamController->Get_OwnerCamMovement();

        CCamSetupBuilder(m_CamController)
            .SetTarget(m_Player->Get_Transform())
            .Distance(12.f)
            .LagSpeed(3.f)
            .ZoomSpeed(2.f)
            .Apply();
    }
}

void CStage_Prologue2::Start_Dialogue()
{
    

}

void CStage_Prologue2::Set_RoomData()
{
    m_RoomData[static_cast<_int>(Room::START)] =
    {
        { 15.1f, 1.f, 43.f },
        "../Bin/Resource/Data/Terrain/Terrain_Tutorial2.map",
        { 0.f, 0.f, 0.f }
    };

    m_RoomData[static_cast<_int>(Room::MIDDLE1)] =
    {
        { 104, 1.f, 11.6f },
        "../Bin/Resource/Data/Terrain/Terrain_Dungeon_Prologue1.map",
        { 100.f, 0.f, 0.f },
        { 117.7f, 1.f, 8.89f }
    };

    m_RoomData[static_cast<_int>(Room::MIDDLE2)] =
    {
        { 210.f, 1.f, 0.f },
        "../Bin/Resource/Data/Terrain/Terrain_Dungeon_Prologue2.map",
        { 200.f, 0.f, 0.f },
        { 213.f, 1.f, 9.986f }
    };

    m_RoomData[static_cast<_int>(Room::END_ROOM)] =
    {
        { 300.f, 1.f, 10.f },
        "../Bin/Resource/Data/Terrain/Terrain_Dungeon_Prologue3.map",
        { 300.f, 0.f, 0.f }
    };

    Set_SpawnPos();
    Connect_Rooms();
    m_CurrentRoom = Room::START;

    for (auto& pair : m_Terrains)
    {
        Room room = pair.first;
        CTerrain* terrain = pair.second;
        if (terrain)
        {
            auto transform = terrain->Get_Transform();

            if (transform)
                transform->Set_Pos(m_RoomData[static_cast<_int>(room)].terrainOffset);

            terrain->LoadMapOnStart(m_RoomData[static_cast<_int>(room)].terrainFilePath.c_str());

            // 보물상자 소환 ㄴ 어차피 인벤토리가 없다. Clear UI도 배제.
            //terrain->Set_ChestSpawnPos(m_RoomData[static_cast<_int>(room)].chestSpawnPos);
        }
    }

    for (auto& pair : m_Terrains)
        pair.second->Set_Active(false);
    m_Terrains[Room::START]->Set_Active(true);

    Create_RoomTriggers();

    _vec3 startPos = m_RoomData[static_cast<_int>(Room::START)].playerSpawnPos;
    m_PlayerTransform->Set_Pos(startPos);
}

void CStage_Prologue2::Rebirth_End()
{
    if (m_IsRebirthPlaying)
    {
        auto playerAnimator = dynamic_cast<CAnimator*>(
            m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

        if (playerAnimator && playerAnimator->Is_AnimationFinished())
        {
            CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Battle1.wav", 0.2f, true, 0.5f);

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

void CStage_Prologue2::Spawn_Ratau()
{
    if (m_Ratau)
        return;

    m_Ratau = CRatau::Create(m_GraphicDev);
    NULL_CHECK(m_Ratau);

    _vec3 ratauPos = _vec3(317.f, 1.f, 5.7f);
    m_Ratau->Get_Transform()->Set_Pos(ratauPos);

    m_Layers[LAYERTYPE::GAMELOGIC]->Add_GameObject(OBJTYPE::NPC, m_Ratau);
    m_Ratau->Initialize_GameObject();

    auto camMovement = m_CamController->Get_OwnerCamMovement();

    CCamSetupBuilder(m_CamController)
        .SetTarget(m_Ratau->Get_Transform())
        .Distance(8.f)                      
        .LagSpeed(3.f)
        .ZoomSpeed(2.f)
        .Apply();

    camMovement->Set_OnTransitionEnd([this]()
        {
            if (m_Dialogue)
                m_Dialogue->Start_Dialogue("Ratau_Start", _vec2(15.f, 180.f), _vec2(250.f, 100.f));
        });

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
