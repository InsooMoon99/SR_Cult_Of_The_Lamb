#pragma once
#include "CScene.h"

class CInventory;
class CWeaponEquip;
class CTerrain;
class CPlayer;

BEGIN(Engine)
class CCombatStat;
class CTransform;
END


struct RoomData
{
    _vec3   spawnPos{};         
    string  terrainFilePath;    
    _vec3   terrainOffset{};    

    // 방 클리어 여부
    // 근데 이거 일단은 구현 X
    bool    isCleared = false;
    _int    requiredKills = 0;
};

class CStage_Dungeon : public CScene
{
public:
    enum class Room
    {
        START,
        MONSTER1,
        MONSTER2,
        TARO,
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

public:
    // TODO : 몬스터, 트리거 등 에디터를 통해 스폰할 수 있도록 변경할 예정
    void    Spawn_RoomObjects(Room room);

    void    Spawn_Weapon();
    void    Spawn_Monster(_int index);

    void    UnLoad_Room(Room room);
    void    Load_Room(Room room);

    void    Create_RoomTriggers();
    void    Create_Trigger_Between(Room fromRoom, Room toRoom,
                _vec3 offsetFromRoom, _vec3 size);

    void Create_Trigger_Between(Room fromRoom,
        _vec3 offsetFromRoom, _vec3 size,
        std::function<void()> callback);

    void    Change_Room(Room room);

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

public:
    static CStage_Dungeon* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CCombatStat*    m_PlayerCombatStatCom;
    CInventory*     m_PlayerInventoryCom;
    CWeaponEquip*   m_WeaponEquipCom;

    map<Room, CTerrain*> m_Terrains {};

private:
    RoomData m_RoomData[5];   // 5개 방 미리 생성
    Room     m_CurrentRoom;   // 현재 방

    CGameObject*    m_Player;
    CTransform*     m_PlayerTransform;

    _bool           m_RequestChangeScene = false;
    SCENETYPE       m_NextSceneType = SCENETYPE::SC_END;

};


