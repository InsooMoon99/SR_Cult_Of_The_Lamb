#include "pch.h"
#include "CNPCState_Run.h"

#include "CAnimator.h"
#include "CCombatStat.h"
#include "CGameObject.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CNPCState_Run::CNPCState_Run(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Run::~CNPCState_Run()
{ }

void CNPCState_Run::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Run";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.05f); // 낮은 마찰 (미끄러지듯 이동)
}

_int CNPCState_Run::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    Find_Player();

    if (m_WalkPastCooldown > 0.f)
        m_WalkPastCooldown -= timeDelta;

    // 플레이어 근처일 경우
    if (m_PlayerTransform && m_OwnerTransform)
    {
        const _vec3  playerPos    = m_PlayerTransform->Get_Pos();
        const _vec3  myPos        = m_OwnerTransform->Get_Pos();
        const _vec3  vecToPlayer  = playerPos - myPos;
        const _float distToPlayer = D3DXVec3Length(&vecToPlayer);

        if (distToPlayer < 2.0f && m_WalkPastCooldown <= 0.f)
        {
            m_WalkPastCooldown = 5.f; // 쿨타임 설정
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::WALKPAST);
        }
    }

    // 이동 로직
    if (m_Controller && m_OwnerTransform && m_OwnerRigidbody)
    {
        const _vec3  targetPos    = m_Controller->Get_TargetPos();
        const _vec3  myPos        = m_OwnerTransform->Get_Pos();
        _vec3        vecToTarget  = targetPos - myPos;
        const _float distToTarget = D3DXVec3Length(&vecToTarget);

        // 건축
        if (distToTarget < 0.2f)
        {
            CGameObject* targetObj = m_Controller->Get_TargetObject();
            if (targetObj && !targetObj->Is_PendingDestroy())
                return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);
            else
                return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }

        D3DXVec3Normalize(&vecToTarget, &vecToTarget);

        // ACTORDIR 설정
        if (m_Owner)
        {
            auto state = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

            if (state)
            {
                if (vecToTarget.x > 0.1f) state->Change_Dir(ACTORDIR::RIGHT);
                else if (vecToTarget.x < -0.1f) state->Change_Dir(ACTORDIR::LEFT);
            }
        }

        _float speed = 0.f;

        if (m_Controller)
        {
            auto stat = m_Controller->Get_OwnerCombatStat();
            if (stat) speed = stat->Get_Speed();
        }

        m_OwnerRigidbody->Add_Force(vecToTarget * speed * 5.f);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
}

void CNPCState_Run::Exit()
{
    CNPCState::Exit();

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f); // 기본 마찰 복구 (정지 상태는 마찰이 높아야 함)
}

HRESULT CNPCState_Run::Ready_State()
{
    return S_OK;
}

CNPCState_Run* CNPCState_Run::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Run(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Run::Free()
{
    CNPCState::Free();
}
