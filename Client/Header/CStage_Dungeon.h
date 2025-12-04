#pragma once
#include "CScene.h"

class CInventory;
class CWeaponEquip;
class CTerrain;

BEGIN(Engine)
class CCombatStat;
END

class CPlayer;

struct RoomData
{
    _vec3   centerPos{};
    _vec3   spawnPos{};
    _float  width = 0.f;
    _float  height = 0.f;

    string  terrainFilePath;
    _vec3   terrainOffset;
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

    void    Load_Room(Room room);
    void    unloadRoom(Room room);

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

};

