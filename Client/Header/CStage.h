#pragma once
#include "CScene.h"

class CInventory;
class CWeaponEquip;

BEGIN(Engine)
class CCombatStat;
END

class CPlayer;

class CStage : public CScene
{
private:
    explicit CStage(DEVICE graphicDev);
    ~CStage() override;

public:
    HRESULT Ready_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

    // void AddObjectOnLayer(LAYERTYPE layerType, CGameObject* obj, OBJTYPE objType);

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

public:
    static CStage* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    //CUIPlayerPanel* m_playerPanel;
    //CPlayer*        m_player;

    // 플레이어는 GAMELOGIC 안에서 만들고 CStage가 직접 참고할 수 없는 오브젝트이므로
    // 그 대신 플레이어 생성 시 Ready_GameLogic_Layer 쪽에서 플레이어의 스테이터스를 저장한 부품인
    // CCombatStat 만을 어쩔수 없이 여기 보관하고, 사용 후 바로 제거하는 의도
    CCombatStat*  m_PlayerCombatStatCom;
    CInventory*   m_PlayerInventoryCom;
    CWeaponEquip* m_WeaponEquipCom;
    CPlayer* m_Player;
};
