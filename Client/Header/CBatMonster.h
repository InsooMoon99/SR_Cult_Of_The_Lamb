#pragma once

#include "CMonsterBase.h"

class CUI_MonsterHp;

BEGIN(Engine)
class CTexture;
class CRigidbody;
class CAttackBox;
END

class CBatMonster : public CMonsterBase
{
private:
    explicit CBatMonster(DEVICE graphicDev);
    explicit CBatMonster(const CBatMonster& rhs);
    ~CBatMonster() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    // 공통 체크 (넉백, 사망)
    _bool   Check_CommonTransitions();
    void     Setup_SpawnDeadParticle();

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


private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

    void     Wandering_Around(const _float timeDelta);
        
public:
    static CBatMonster* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CRigidbody*     m_RigidbodyCom;
    _bool           m_AttackSpawned;

    CUI_MonsterHp*  m_HpBar;

    _vec3           m_ChargeDir = _vec3(0.f, 0.f, 0.f);        
    _float          m_ChargeDistance = 0.f;  
    _float          m_ChargedDistance = 0.f;

private:
    _float          m_WanderAngle = 0.f;     
    _float          m_WanderRadius = 3.f;    
    _float          m_WanderSpeed = 0.25f;

    _float          m_HoverTime = 0.f;        
    _float          m_HoverAmplitude = 0.3f;  
    _float          m_HoverSpeed = 5.f;       

    CAttackBox*     m_AttackBox = nullptr;

    _vec3  m_MoveVel = _vec3(0.f, 0.f, 0.f); 
    _float m_MoveAccel = 220.f;
};
