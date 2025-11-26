#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossDead :
    public CMonsterState
{
public:
    enum class PHASE { DOWN, TALK , DISAPPEAR };
    PHASE m_ePhase;
private:
    explicit CBossDead(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossDead();

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
    static CBossDead* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();

};

