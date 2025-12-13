#include "pch.h"
#include "CNPCState_ChopWood.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"

CNPCState_ChopWood::CNPCState_ChopWood(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_ChopWood::~CNPCState_ChopWood()
{ }

void CNPCState_ChopWood::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"ChopWood";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_ChopWood::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::CHOPWOOD);
}

void CNPCState_ChopWood::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_ChopWood::Ready_State()
{
    return S_OK;
}

CNPCState_ChopWood* CNPCState_ChopWood::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_ChopWood(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_ChopWood::Free()
{
    CNPCState::Free();
}
