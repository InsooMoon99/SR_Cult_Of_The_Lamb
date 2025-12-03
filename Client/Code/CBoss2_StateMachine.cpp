#include "pch.h"
#include "CBoss2_StateMachine.h"
#include "CBossPhase2.h"
#include "CMonsterState.h"
#include "CBoss2_Idle.h"
#include "CBoss2_Eyeball.h"

CBoss2_StateMachine::CBoss2_StateMachine(CMonsterBase* owner)
    : m_Owner(owner)
{
}

CBoss2_StateMachine::~CBoss2_StateMachine()
{
    Free();
       
}

HRESULT CBoss2_StateMachine::Ready_StateMachine()
{
    m_vecState.resize(CBossPhase2::PATTERN::PHASE2_END);

    CMonsterState* state = nullptr;

    NULL_CHECK_RETURN(m_Owner, E_FAIL);

#pragma region 상태 등록
    {
        // Idle
        if (FAILED(RegisterState<CBoss2_Idle>(CBossPhase2::PATTERN::PHASE2_IDLE, m_Owner)))
            return E_FAIL;

        // Eyeball
        if (FAILED(RegisterState<CBoss2_Eyeball>(CBossPhase2::PATTERN::PHASE2_EYEBALL, m_Owner)))
            return E_FAIL;
    }
#pragma endregion
    

    // 초기 상태 설정
    m_pCurState = m_vecState[CBossPhase2::PATTERN::PHASE2_IDLE];
    m_dwCurStateKey = CBossPhase2::PATTERN::PHASE2_IDLE;
    m_pCurState->Enter();

    return S_OK;
}

_int CBoss2_StateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_pCurState)
    {
        _uint nextState = m_pCurState->Update_State(timeDelta);

        if (nextState < CBossPhase2::PATTERN::PHASE2_END && nextState != m_dwCurStateKey)
        {
            Change_State(nextState);
        }
    }

    return 0;
}

void CBoss2_StateMachine::Change_State(_uint dwStateKey)
{
    if (m_dwCurStateKey == dwStateKey)
        return;

    if (m_vecState[dwStateKey] == nullptr || dwStateKey >= CBossPhase2::PATTERN::PHASE2_END)
        return;

    // 기존 상태 정리
    if (m_pCurState)
        m_pCurState->Exit();

    m_dwPrevStateKey = m_dwCurStateKey;
    m_dwCurStateKey = dwStateKey;

    m_pCurState = m_vecState[dwStateKey];
    m_pCurState->Enter();
}

CBoss2_StateMachine* CBoss2_StateMachine::Create(CMonsterBase* owner)
{
    CBoss2_StateMachine* stateMachine = new CBoss2_StateMachine(owner);

    if (FAILED(stateMachine->Ready_StateMachine()))
    {
        Safe_Release(stateMachine);
        MSG_BOX("Boss2 State Machine Create Failed");
        return nullptr;
    }

    return stateMachine;
}

void CBoss2_StateMachine::Free()
{
    CStateMachine::Free();

}
