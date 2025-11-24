#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CBossPhase1;
class CMonsterState;
class CBossStateMachine :
    public CBase
{
protected:
    explicit CBossStateMachine(CBossPhase1* owner);
    virtual ~CBossStateMachine();

public:
    virtual HRESULT Ready_StateMachine();
    _int    Update_StateMachine(const _float& timeDelta);
    virtual void Change_State(_uint changeStateKey);


    CMonsterState* Get_CurState() { return m_pCurState; }
protected:
    vector<CMonsterState*> m_vecState;
    CMonsterState* m_pCurState;

    _uint                   m_dwPrevStateKey;
    _uint                   m_dwCurStateKey;

private:
    CBossPhase1* m_pOwner;

protected:
    HRESULT Ready_BossStates();
public:
    static CBossStateMachine* Create(CBossPhase1* owner);

protected:
    virtual void Free();
};

