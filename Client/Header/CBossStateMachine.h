#pragma once
#include "CBase.h"
#include "CStateMachine.h"

class CBossPhase1;
class CMonsterState;
class CMonsterBase;

class CBossStateMachine : public CStateMachine
{
protected:
    explicit CBossStateMachine(CMonsterBase* owner);
    virtual ~CBossStateMachine();

public:
    virtual HRESULT Ready_StateMachine();
    _int            Update_StateMachine(const _float& timeDelta) override;
    virtual void    Change_State(_uint changeStateKey);
    _int            GetCurStateNum();

    CBaseState*     Get_CurState() { return m_pCurState; }

private:
    CMonsterBase* m_pOwner;

protected:
    HRESULT Ready_BossStates();

public:
    static CBossStateMachine* Create(CMonsterBase* owner);

protected:
    virtual void Free();
};

