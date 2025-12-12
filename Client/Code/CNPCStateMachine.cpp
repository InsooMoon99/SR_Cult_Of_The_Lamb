#include "pch.h"
#include "CNPCStateMachine.h"
#include "CBaseState.h"

#include "CNPCState_Idle.h"
#include "CNPCState_Run.h"
#include "CNPCState_WalkPast.h"
#include "CNPCState_Build.h"
#include "CNPCState_Spawn.h"

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

    if (FAILED(RegisterState<CNPCState_WalkPast>(NPC_STATE::WALKPAST, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Build>(NPC_STATE::BUILD, m_Owner)))
        return E_FAIL;

    // 초기 상태
    m_CurState     = m_vecState[static_cast<_uint>(NPC_STATE::IDLE)];
    m_CurStateKey = static_cast<_uint>(NPC_STATE::IDLE);
    m_CurState->Enter();

    return S_OK;
}

_int CNPCStateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState != m_CurStateKey)
            Change_State(nextState);
    }

    return 0;
}

void CNPCStateMachine::Change_State(_uint dwStateKey)
{
    if (m_CurStateKey == dwStateKey)
        return;

    // 범위 체크 및 nullptr 체크
    if (dwStateKey >= m_vecState.size() || m_vecState[dwStateKey] == nullptr)
        return;

    // 기존 상태 정리
    if (m_CurState)
        m_CurState->Exit();

    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey  = dwStateKey;

    m_CurState = m_vecState[dwStateKey];
    m_CurState->Enter();
}

CNPCStateMachine* CNPCStateMachine::Create(CNPCBase* owner)
{
    auto instance = new CNPCStateMachine;
    instance->Set_Owner(owner);

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
