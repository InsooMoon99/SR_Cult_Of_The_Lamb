#pragma once

#include "CMonsterBase.h"

class CUI_MonsterHp;

BEGIN(Engine)
class CTexture;
class CRigidbody;
END

class CHumanMonster : public CMonsterBase
{
private:
    explicit CHumanMonster(DEVICE graphicDev);
    explicit CHumanMonster(const CHumanMonster& rhs);
    ~CHumanMonster() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    // 공통 체크 (넉백, 사망)
    _bool   Check_CommonTransitions();

    // 상태 변환
    void    Update_State_Idle(const _float timeDelta);
    void    Update_State_Run(const _float timeDelta);
    void    Update_State_Attack(const _float timeDelta);
    void    Update_State_Hit(const _float timeDelta);
    void    Update_State_Dead(const _float timeDelta);

    void    Update_Direction();

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    void    Chase_Target(const float& timeDelta, _float speed) override;
    void    Retreat_FromTarget(const _float& timeDelta, _float speed) override;

    void Enable_Battle() { m_BattleEnabled = true; }
    void Set_Battle(_bool b) { m_BattleEnabled = b; }
    bool Is_BattleEnabled() const { return m_BattleEnabled; }

public:
    void    Set_PrayMode(_bool pray);
    _bool   Is_PrayMode() const { return m_IsPraying; }

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    static CHumanMonster* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CRigidbody*     m_RigidbodyCom;
    _bool           m_AttackSpawned;

    bool            m_BattleEnabled = true;

    CUI_MonsterHp*  m_HpBar;
    _bool           m_IsPraying = false;

};
