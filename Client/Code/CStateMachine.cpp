#include "pch.h"
#include "CStateMachine.h"
#include "CBaseState.h"

CStateMachine::CStateMachine() :m_pCurState(nullptr),m_dwPrevStateKey(0),m_dwCurStateKey(0)
{
}

CStateMachine::~CStateMachine()
{
}

void CStateMachine::Free()
{
    for_each(m_vecState.begin(), m_vecState.end(), [](CBaseState*& state)
        {
        Safe_Release(state); 
        });
    m_vecState.clear();
}
