#include "pch.h"
#include "CNPCState_WalkPast.h"

#include "CAnimator.h"
#include "CCombatStat.h"
#include "CGameObject.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CNPCState_WalkPast::CNPCState_WalkPast(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_WalkPast::~CNPCState_WalkPast()
{ }

void CNPCState_WalkPast::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        // "Walk" -> "WalkPast"
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"WalkPast";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.1f); 
}

_int CNPCState_WalkPast::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // Animation Finish Check
    if (m_OwnerAnimator && m_OwnerAnimator->Is_AnimationFinished())
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);

    // 이동 로직
    if (m_Controller && m_OwnerTransform && m_OwnerRigidbody)
    {
        const _vec3  targetPos    = m_Controller->Get_TargetPos();
        const _vec3  myPos        = m_OwnerTransform->Get_Pos();
        _vec3        vecToTarget  = targetPos - myPos;
        const _float distToTarget = D3DXVec3Length(&vecToTarget);

        // Arrival Check
        if (distToTarget < 0.2f)
        {
            // If we have a valid target object (construction), go to BUILD
            CGameObject* targetObj = m_Controller->Get_TargetObject();
            if (targetObj && !targetObj->Is_PendingDestroy())
                return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);
            else
                return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }

        D3DXVec3Normalize(&vecToTarget, &vecToTarget);

        // Direction
        if (m_Owner)
        {
            auto state = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
            if (state)
            {
                if (vecToTarget.x > 0.1f) state->Change_Dir(ACTORDIR::RIGHT);
                else if (vecToTarget.x < -0.1f) state->Change_Dir(ACTORDIR::LEFT);
            }
        }

        // Speed: 50% of Run
        _float speed = 3.0f;
        if (m_Controller)
        {
            auto stat = m_Controller->Get_OwnerCombatStat();
            if (stat) speed = stat->Get_Speed();
        }

        speed *= 0.5f; // Slower

        m_OwnerRigidbody->Add_Force(vecToTarget * speed * 5.f);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::WALKPAST);
}

void CNPCState_WalkPast::Exit()
{
    CNPCState::Exit();

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

HRESULT CNPCState_WalkPast::Ready_State()
{
    return S_OK;
}

CNPCState_WalkPast* CNPCState_WalkPast::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_WalkPast(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_WalkPast::Free()
{
    CNPCState::Free();
}
