#include "pch.h"
#include "CNPCState_Run.h"

#include "CAnimator.h"
#include "CNPCStateMachine.h"

CNPCState_Run::CNPCState_Run(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Run::~CNPCState_Run()
{ }

void CNPCState_Run::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator)
        m_OwnerAnimator->Play_Animation(L"CowRun", ANIMSTATE::LOOP);
}

_int CNPCState_Run::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
}

void CNPCState_Run::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Run::Ready_State()
{
    return S_OK;
}

CNPCState_Run* CNPCState_Run::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Run(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Run::Free()
{
    CNPCState::Free();
}
