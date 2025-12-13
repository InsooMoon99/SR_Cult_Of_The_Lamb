#include "pch.h"
#include "CNPCState_PrayPeek.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"

CNPCState_PrayPeek::CNPCState_PrayPeek(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_PrayPeek::~CNPCState_PrayPeek()
{ }

void CNPCState_PrayPeek::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"PrayPeek_Half";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_PrayPeek::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::CHOPWOOD);
}

void CNPCState_PrayPeek::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_PrayPeek::Ready_State()
{
    return S_OK;
}

CNPCState_PrayPeek* CNPCState_PrayPeek::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_PrayPeek(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_PrayPeek::Free()
{
    CNPCState::Free();
}
