#include "pch.h"
#include "CNPCState_Pray.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"

CNPCState_Pray::CNPCState_Pray(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Pray::~CNPCState_Pray()
{ }

void CNPCState_Pray::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Pray_Half";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_Pray::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::CHOPWOOD);
}

void CNPCState_Pray::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Pray::Ready_State()
{
    return S_OK;
}

CNPCState_Pray* CNPCState_Pray::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Pray(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Pray::Free()
{
    CNPCState::Free();
}
