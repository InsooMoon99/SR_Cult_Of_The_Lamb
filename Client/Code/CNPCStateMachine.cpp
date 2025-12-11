#include "pch.h"
#include "CNPCStateMachine.h"
#include "CBaseState.h"

#include "CNPCState_Idle.h"
#include "CNPCState_Run.h"

CNPCStateMachine::CNPCStateMachine()
{ }

CNPCStateMachine::~CNPCStateMachine()
{ }

HRESULT CNPCStateMachine::Ready_StateMachine()
{
    m_vecState.resize(static_cast<_uint>(NPC_STATE::STATE_END));

    if (FAILED(RegisterState<CNPCState_Idle>(NPC_STATE::IDLE, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Run>(NPC_STATE::RUN, m_Owner)))
        return E_FAIL;

    // 초기 상태
    m_pCurState     = m_vecState[static_cast<_uint>(NPC_STATE::IDLE)];
    m_dwCurStateKey = static_cast<_uint>(NPC_STATE::IDLE);
    m_pCurState->Enter();

    return S_OK;
}

_int CNPCStateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_pCurState)
    {
        _uint nextState = m_pCurState->Update_State(timeDelta);

        if (nextState != m_dwCurStateKey)
            Change_State(nextState);
    }

    return 0;
}

void CNPCStateMachine::Change_State(_uint dwStateKey)
{
    if (m_dwCurStateKey == dwStateKey)
        return;

    // 범위 체크 및 nullptr 체크
    if (dwStateKey >= m_vecState.size() || m_vecState[dwStateKey] == nullptr)
        return;

    // 기존 상태 정리
    if (m_pCurState)
        m_pCurState->Exit();

    m_dwPrevStateKey = m_dwCurStateKey;
    m_dwCurStateKey  = dwStateKey;

    m_pCurState = m_vecState[dwStateKey];
    m_pCurState->Enter();
}

CNPCStateMachine* CNPCStateMachine::Create()
{
    auto instance = new CNPCStateMachine;

    if (FAILED(instance->Ready_StateMachine()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCStateMachine::Free()
{
    CStateMachine::Free();
}
