#include "pch.h"
#include "CNPCState_Mining.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"

CNPCState_Mining::CNPCState_Mining(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Mining::~CNPCState_Mining()
{ }

void CNPCState_Mining::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Mining";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_Mining::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::MINING);
}

void CNPCState_Mining::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Mining::Ready_State()
{
    return S_OK;
}

CNPCState_Mining* CNPCState_Mining::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Mining(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Mining::Free()
{
    CNPCState::Free();
}
