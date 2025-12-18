#pragma once
#include "CScene.h"

class CUI_DungeonLootPanel;
class CUI_Interactive;
class CRatau;
class CCameraController;
class CTerrain;
class CUI_Dialogue;
class CPlayer;

class CHeket;
class CKallamar;
class CLeshy;
class CShamura;

class CUI_Fade;

BEGIN(Engine)

class CTransform;
class CRigidbody;

END

struct PrologueRoomData
{
    _vec3   playerSpawnPos{};
    string  terrainFilePath;
    _vec3   terrainOffset{};
    _vec3   chestSpawnPos;
};

class CStage_Prologue2 : public CScene
{
private:
    enum class Room
    {
        START,
        MIDDLE1,
        MIDDLE2,
        END_ROOM,
        PROLOGUE2_END
    };

private:
    explicit CStage_Prologue2(DEVICE graphicDev);
    ~CStage_Prologue2() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;
    HRESULT Ready_Light() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

    void Create_RoomTriggers();
    void Create_BiDirectional_Trigger(Room roomA, _vec3 offsetA, Room roomB, _vec3 offsetB, _vec3 size);

    void Change_Room(Room room);
    void Fade_Check(const _float timeDelta);

    void Connect_Rooms();
    void Set_SpawnPos();
    void Setup_DialogueCallbacks();

    void OnDialogueEnd(const string& id);
    void Start_Dialogue();

    void Set_RoomData();


private:
    void    Rebirth_End();
    void    Spawn_Ratau();

public:
    static CStage_Prologue2* Create(DEVICE graphicDev);
    
    _bool IsCleared() const { return m_PrologueCleared; }

private:
    void Free() override;

private:
    CTerrain*            m_Terrain;

    CTransform*          m_PlayerTransform = nullptr;
    CRigidbody*          m_PlayerRigid = nullptr;
    CUI_Dialogue*        m_Dialogue = nullptr;
    CGameObject*         m_Player = nullptr;

    CCameraController*   m_CamController = nullptr;
    _bool                m_IsRebirthPlaying = false;

    vector<CGameObject*> m_Monsters;

    D3DXCOLOR            m_LightDiffuse = D3DXCOLOR(0.631f, 0.345f, 0.345f, 1.0f);
    D3DXCOLOR            m_LightAmbient = D3DXCOLOR(0.710f, 0.463f, 0.463f, 1.0f);
    D3DXCOLOR            m_LightSpecular = D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f);
    D3DXVECTOR3          m_LightDir = D3DXVECTOR3(0.f, -1.f, 1.f);

private:
    map<Room, CTerrain*>            m_Terrains;
    map<Room, vector<Room>>         m_ConnectRooms;     // 연동된 방 판단. 그런데 일단 필요 ?
    map<pair<Room, Room>, _vec3>    m_SpawnPositions;

    PrologueRoomData    m_RoomData[static_cast<_int>(Room::PROLOGUE2_END)];
    Room                m_CurrentRoom;
    Room                m_FromRoom = Room::PROLOGUE2_END;
    Room                m_NextRoom;

    CUI_Fade*           m_UI_Fade = nullptr;
    _bool               m_IsFading = false;

    CRatau*             m_Ratau = nullptr;

private:
   //CUI_Interactive*        m_UI_Interact = nullptr;
   //CUI_DungeonLootPanel*   m_ClearPanel = nullptr;
    _bool                   m_InWarpZone = false;
    _bool                   m_WarpStarted = false;
    _bool                   m_PrologueCleared = false;
};
