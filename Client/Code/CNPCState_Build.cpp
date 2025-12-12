#include "pch.h"
#include "CNPCState_Build.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CGameObject.h"

CNPCState_Build::CNPCState_Build(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Build::~CNPCState_Build()
{ }

void CNPCState_Build::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Build";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }
}

_int CNPCState_Build::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // Check Target Validity
    if (m_Controller)
    {
        CGameObject* target = m_Controller->Get_TargetObject();
        if (!target || target->Is_PendingDestroy())
        {
            // Building finished or cancelled
            m_Controller->Set_TargetObject(nullptr);
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }
    }
    else
    {
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
    }

    // Actual progress logic is handled by CConstructCircle::OnStayOverlap checking this state

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);
}

void CNPCState_Build::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Build::Ready_State()
{
    return S_OK;
}

CNPCState_Build* CNPCState_Build::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Build(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Build::Free()
{
    CNPCState::Free();
}
