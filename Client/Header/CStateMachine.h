#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CBaseState;

class CStateMachine :
    public CBase
{
protected:
    explicit CStateMachine();
    virtual ~CStateMachine();

public:
    virtual HRESULT       Ready_StateMachine() = 0;
    virtual _int          Update_StateMachine(const _float& timeDelta) = 0;
    virtual void          Change_State(_uint dwStateKey) = 0;

    _uint Get_CurStateNum() { return m_dwCurStateKey; }
   
protected:
    vector<CBaseState*> m_vecState;
    CBaseState* m_pCurState;
   // CBaseState* m_pPrevState;

    _uint                   m_dwPrevStateKey;
    _uint                   m_dwCurStateKey;

protected:
    virtual void Free();
};

