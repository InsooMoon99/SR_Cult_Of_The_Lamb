#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossBulletAttack :
    public CMonsterState
{
public:
    enum class PHASE { CIRCLE, ROTCROSS_SPREAD, SPREAD ,PHASE_END};

private:
    explicit CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossBulletAttack();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossBulletAttack* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    PHASE m_ePhase;
    float m_fElapsedTime[(int)PHASE::PHASE_END];
private:
    virtual void Free();

};

