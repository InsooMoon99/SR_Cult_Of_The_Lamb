#pragma once
#include "CMonsterState.h"

class CEligos;
class CEligos_StateMachine;

BEGIN(Engine)
class CTexture;
class CRigidbody;
END

class CEligos_Attack_Melee :
    public CMonsterState
{
public:
    enum class PHASE { SHOOT, READY };
    PHASE m_ePhase;
private:
    explicit CEligos_Attack_Melee(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Attack_Melee();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

    bool Attack_Melee();

private:
    CEligos* m_pOwner;
    CEligos_StateMachine* m_pStateMachine;

    CRigidbody* m_pOwnerRigidbodyCom;

public:
    static CEligos_Attack_Melee* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();


    bool  m_AttackSpawned = false;

    _vec3 tmpOwnerPos;
    _vec3 m_TargetDir;

    bool m_flag = false;
    float m_fDashTime = 0;
};

