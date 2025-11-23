#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CProtoMgr;
class CRectCollider;
class CBoxCollider;
class CState;
class CCombatStat;
END

class CWeaponEquip;
class CPlayerController;

class CInventory; // 이건 클라에 있음

class CPlayer : public CGameObject
{
private:
    explicit CPlayer(DEVICE graphicDev);
    explicit CPlayer(const CPlayer& rhs);
    ~CPlayer() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void          OnBeginOverlap(CCollider* self, CCollider* other) override;
    void          OnEndOverlap(CCollider* self, CCollider* other) override;
    CCombatStat*  Get_CombatStat() { return m_CombatStatCom; }
    CWeaponEquip* Get_WeaponEquip() { return m_WeaponEquipCom; }

    void Render_Editor() override;

private:
    HRESULT Add_Component();
    void    Animation_Setting();

    void Render_Setting();
    void Render_Reset();

private:
#pragma region Engine Components

    Engine::CRcTex*       m_BufferCom;
    Engine::CTransform*   m_TransformCom;
    Engine::CTexture*     m_TextureCom;
    Engine::CAnimator*    m_AnimatorCom;
    Engine::CBoxCollider* m_BoxColCom;
    Engine::CState*       m_StateCom;
    Engine::CCombatStat*  m_CombatStatCom;

#pragma endregion
#pragma region Client Components

    CWeaponEquip*      m_WeaponEquipCom;
    CInventory*        m_Inventory;
    CPlayerController* m_PlayerControllerCom;

#pragma endregion

    map<ACTORSTATE, wstring> m_StateAnim;

public:
    static CPlayer* Create(DEVICE graphicDev);

private:
    void Free() override;
};
