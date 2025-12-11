#include "pch.h"
#include "CNPCState_Idle.h"

#include "CAnimator.h"
#include "CNPCStateMachine.h"

CNPCState_Idle::CNPCState_Idle(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Idle::~CNPCState_Idle()
{ }

void CNPCState_Idle::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator)
        m_OwnerAnimator->Play_Animation(L"CowIdle", ANIMSTATE::LOOP);
}

_int CNPCState_Idle::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // TODO: Transition logic

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
}

void CNPCState_Idle::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Idle::Ready_State()
{
    return S_OK;
}

CNPCState_Idle* CNPCState_Idle::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Idle(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Idle::Free()
{
    CNPCState::Free();
}
