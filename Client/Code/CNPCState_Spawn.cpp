#include "pch.h"
#include "CNPCState_Spawn.h"

#include "CAnimator.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CSpriteAnimation.h"

CNPCState_Spawn::CNPCState_Spawn(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Spawn::~CNPCState_Spawn()
{ }

void CNPCState_Spawn::Enter()
{
    CNPCState::Enter();

    // 생성 애니메이션 (Prefix + Spawn_Half)
    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Spawn_Half";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }
}

_int CNPCState_Spawn::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // 애니메이션 종료 시 IDLE로 전환
    if (m_OwnerAnimator && m_OwnerAnimator->Get_CurAnimation()->Get_AnimState() == ANIMSTATE::ONCE)
    {
        if (m_OwnerAnimator->Is_AnimationFinished())
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::SPAWN);
}

void CNPCState_Spawn::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Spawn::Ready_State()
{
    return S_OK;
}

CNPCState_Spawn* CNPCState_Spawn::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Spawn(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Spawn::Free()
{
    CNPCState::Free();
}
