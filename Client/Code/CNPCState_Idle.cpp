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
        // 상호작용 검사 작동
        CInteractionChecker* checker = m_Owner->Get_InteractionChecker();

        // 건축 부지 탐색
        if (checker)
        {
            _vec3             pos        = m_OwnerTransform->Get_Pos();
            CConstructCircle* nearCircle = checker->Get_NearestConstruction(pos);

            if (nearCircle)
            {
                // 건축 부지 탐색
                if (m_Controller)
                {
                    _vec3 vTargetPos = nearCircle->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(vTargetPos);
                    m_Controller->Set_TargetObject(nearCircle); // Set Target Object

                    _vec3 toTarget = vTargetPos - pos;

                    _float dist = D3DXVec3Length(&toTarget);

                    if (dist < 0.2f) 
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);
                    else
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }

            //TODO 직업에 따라 벌목 또는 채광 진행
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
