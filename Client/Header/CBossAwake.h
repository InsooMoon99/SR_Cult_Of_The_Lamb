#pragma once
#include "CMonsterState.h"
class CBossAwake :
    public CMonsterState
{
    explicit CBossAwake();
    virtual ~CBossAwake();

public:
    virtual void Enter();
    virtual void Exit();


public:
     virtual HRESULT Ready_State();
     virtual _int    Update_State(const _float& timeDelta);
     virtual void    LateUpdate_State(const _float& timeDelta);
     virtual void    Render_State() {}

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossAwake* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();
};

