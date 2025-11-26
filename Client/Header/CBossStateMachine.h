#pragma once
#include "CBase.h"
#include "CStateMachine.h"

class CBossPhase1;
class CMonsterState;

class CBossStateMachine :
    public CStateMachine
{
protected:
    explicit CBossStateMachine(CBossPhase1* owner);
    virtual ~CBossStateMachine();

public:
    virtual HRESULT Ready_StateMachine();
    _int    Update_StateMachine(const _float& timeDelta);
    virtual void Change_State(_uint changeStateKey);
    _int GetCurStateNum();

    CBaseState* Get_CurState() { return m_pCurState; }

private:
    CBossPhase1* m_pOwner;

protected:
    HRESULT Ready_BossStates();

public:
    static CBossStateMachine* Create(CBossPhase1* owner);

protected:
    virtual void Free();
};

