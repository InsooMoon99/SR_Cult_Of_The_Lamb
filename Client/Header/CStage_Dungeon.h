#pragma once
#include "CRunLootLog.h"
#include "CScene.h"

class CInventory;
class CWeaponEquip;
class CTerrain;
class CPlayer;
class CUI_Fade;
class CCameraMovement;
class CCameraController;
class CTarotNPC;
class CUI_BossHp;

BEGIN(Engine)
class CCombatStat;
class CTransform;

END

struct RoomData
{
    _vec3  playerSpawnPos{};
    string terrainFilePath;
    _vec3  terrainOffset{};
    _vec3  chestSpawnPos;
};

class CStage_Dungeon : public CScene
{
public:
    enum class Room
    {
        START,
        MONSTER1,
        MONSTER2,
        MONSTER3,
        TARO,
        MONSTER4,
        MONSTER5,
        BOSS,

        DUNGEON_END
    };

private:
    explicit CStage_Dungeon(DEVICE graphicDev);
    ~CStage_Dungeon() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;
    HRESULT Ready_Light() override;

public:
    void Spawn_Weapon();
    void Spawn_FollowerCow_Rescue(_vec3* spawnPos = nullptr);
    void Spawn_FollowerElephant_Rescue(_vec3* spawnPos = nullptr);
    void Spawn_FollowerPig_Rescue(_vec3* spawnPos = nullptr);

    void Create_RoomTriggers();
    void Create_Trigger_Between(Room  fromRoom,
                                Room  toRoom,
                                _vec3 offsetFromRoom,
                                _vec3 size);

    void Create_Trigger_Between(Room             fromRoom,
                                _vec3            offsetFromRoom,
                                _vec3            size,
                                function<void()> callback);

    // 양방향 가능 트리거
    void Create_BiDirectional_Trigger(
        Room  roomA,
        _vec3 offsetA,    // A방 출구 위치
        Room  roomB,
        _vec3 offsetB,    // B방 출구 위치
        _vec3 size);

    void EnterDungeon();
    void Change_Room(Room room);

    CRunLootLog& Get_RunLootLog() { return m_RunLootLog; }
    _float       Get_Timer() { return m_Timer; }
    void         Set_TimerRunning(bool timerRunning) { m_TimerRunning = timerRunning; }

    // 몬스터 죽었을 때 호출
    void  OnMonsterDeath(CGameObject* obj) override;
    _bool IsCleared() { return m_DungeonCleared; }

private:
    const CRunLootLog& Get_RunLootLog() const { return m_RunLootLog; }
private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

public:
    static CStage_Dungeon* Create(DEVICE graphicDev);

private:
    void Free() override;
    void Fade_Check(const _float timeDelta);
    void Connect_Rooms();
    void Set_SpawnPos();

    void Setup_DynamicCam_For_Boss();

private:
    CCombatStat*  m_PlayerCombatStatCom;
    CInventory*   m_PlayerInventoryCom;
    CWeaponEquip* m_WeaponEquipCom;

    map<Room, CTerrain*> m_Terrains{};

    // 연결된 방들
    map<Room, vector<Room>> m_ConnectRooms;

    map<pair<Room, Room>, _vec3> m_SpawnPositions;

private:
    RoomData m_RoomData[static_cast<_int>(Room::DUNGEON_END)];
    Room     m_CurrentRoom;   // 현재 방
    Room     m_FromRoom = Room::DUNGEON_END;

    CGameObject* m_Player;
    CTransform*  m_PlayerTransform;

    _bool     m_RequestChangeScene = false;
    SCENETYPE m_NextSceneType      = SCENETYPE::SC_END;

    CUI_Fade* m_UI_Fade;

private:
    _bool m_IsFading = false;
    Room  m_NextRoom;

    _float m_PrevZoomSpeed = 0.f;

    // 카메라 무브먼트
    CCameraMovement*   m_CamMovement;
    CCameraController* m_CamController;
    CRunLootLog        m_RunLootLog;
    _float             m_Timer;

    _bool m_TimerRunning;
    _bool m_DungeonCleared = false;
    _bool m_IsClearMenuOpened;
    _bool m_Start;

    CGameObject* m_Boss      = nullptr;
    CUI_BossHp*  m_BossHpbar = nullptr;

    bool m_flag = false;
};
