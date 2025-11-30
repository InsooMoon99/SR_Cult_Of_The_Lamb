#pragma once
#include "CGameObject.h"

class CWeaponEquip;
class CInventory;
class CUIInventory;
class CPlayerController;
class CPlayerInput;
class CPlayerMovement;
class CPlayerAttack;

BEGIN(Engine)
class CRigidbody;
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CProtoMgr;
class CRectCollider;
class CBoxCollider;
class CSphereCollider;
class CState;
class CCombatStat;
END

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

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    void Render_Editor() override;

public:
    // 테스트 공격용
    // 나중에 Compoennt를 새로 만들던, 클래스로 나누던 자유
    void Test_Attack();

private:
    HRESULT Add_Component();
    void    Animation_Setting();
    void    Render_Setting();
    void    Render_Reset();

private:
#pragma region Engine Components
    Engine::CRcTex*       m_BufferCom;
    Engine::CTransform*   m_TransformCom;
    Engine::CTexture*     m_TextureCom;
    Engine::CAnimator*    m_AnimatorCom;
    Engine::CBoxCollider* m_BoxColCom;
    Engine::CState*       m_StateCom;
    Engine::CCombatStat*  m_CombatStatCom;
    Engine::CRigidbody*   m_RigidbodyCom;

#pragma endregion
#pragma region Client Components
    CWeaponEquip*      m_WeaponEquipCom;
    CInventory*        m_InventoryCom;
    CUIInventory*      m_UIInventory;
    CPlayerController* m_PlayerControllerCom;
    CPlayerInput*      m_PlayerInputCom;
    CPlayerMovement*   m_PlayerMovementCom;
    CPlayerAttack*     m_PlayerAttackCom;

#pragma endregion

    map<ACTORSTATE, wstring> m_StateAnim;

public:
    static CPlayer* Create(DEVICE graphicDev);

private:
    void Free() override;
};
