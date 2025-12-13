#include "pch.h"
#include "CNPCState_Idle.h"

#include "CAnimator.h"
#include "CConstructCircle.h"
#include "CInteractionChecker.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CTransform.h"

CNPCState_Idle::CNPCState_Idle(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Idle::~CNPCState_Idle()
{ }

void CNPCState_Idle::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Idle";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_Idle::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    if (m_Owner)
    {
        // 1. Check Construction
        CInteractionChecker* checker = m_Owner->Get_InteractionChecker();

        if (checker)
        {
            _vec3             pos        = m_OwnerTransform->Get_Pos();
            CConstructCircle* nearCircle = checker->Get_NearestConstruction(pos);

            if (nearCircle)
            {
                // Found Construction -> Move to it (RUN) -> Build
                if (m_Controller)
                {
                    _vec3 vTargetPos = nearCircle->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(vTargetPos);

                    // 거리 체크 (이미 가까우면 바로 BUILD로 갈 수도 있음)
                    _vec3 toTarget = vTargetPos - pos;

                    _float dist = D3DXVec3Length(&toTarget);

                    if (dist < 1.0f) // 임의의 거리 임계값
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD); // TODO: BUILD State 구현 시 활성화
                    else
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }

            // 2. Check Job (If no construction)
            /*
            JOB eJob = m_pController->Get_Work();
            switch (eJob) {
                case JOB::LUMBERJACK:
                   // Find Tree -> Set Target -> Run or Chop
                   break;
                // ...
            }
            */
        }
    }

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
