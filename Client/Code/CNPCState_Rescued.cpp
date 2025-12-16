#include "pch.h"
#include "CNPCState_Rescued.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"

CNPCState_Rescued::CNPCState_Rescued(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{
}

CNPCState_Rescued::~CNPCState_Rescued()
{
}

void CNPCState_Rescued::Enter()
{
    CNPCState::Enter();

    m_AnimStep = 0;

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Transform1"; 
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }
}

_int CNPCState_Rescued::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    if (m_OwnerAnimator && m_OwnerAnimator->Is_AnimationFinished())
    {
        m_AnimStep++;

        if (m_AnimStep == 1)
        {
            wstring key = m_Controller->Get_AnimKeyPrefix() + L"Transform2";
            m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
        }
        else if (m_AnimStep == 2)
        {
            wstring key = m_Controller->Get_AnimKeyPrefix() + L"Transform3";
            m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
        }
        else if (m_AnimStep > 2)
        {
            if (m_Owner)
                m_Owner->Mark_As_PendingKill();
        }
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RESCUED);
}

void CNPCState_Rescued::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Rescued::Ready_State()
{
    return S_OK;
}

CNPCState_Rescued* CNPCState_Rescued::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Rescued(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Rescued::Free()
{
    CNPCState::Free();
}
