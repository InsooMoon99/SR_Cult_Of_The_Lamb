#include "pch.h"
#include "CNPCState_Run.h"

#include "CAnimator.h"
#include "CCombatStat.h"
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

    // Movement Logic
    if (m_Controller && m_OwnerTransform && m_OwnerRigidbody)
    {
        _vec3  targetPos = m_Controller->Get_TargetPos();
        _vec3  myPos     = m_OwnerTransform->Get_Pos();
        _vec3  dir       = targetPos - myPos;
        _float dist      = D3DXVec3Length(&dir);

        if (dist < 0.2f) // Arrival Threshold
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);

        D3DXVec3Normalize(&dir, &dir);

        // Change Direction (Left/Right only for now)
        // Use CCharacterBase::Change_Dir (via Owner -> Get_State -> Change_Dir) or CCombatStat logic?
        // CNPCBase inherits CCharacterBase, which has m_StateCom (CState).
        // CState manages ACTORDIR.
        // We need to access Owner's State Component.
        // Assuming CMonsterController or CNPCController provides access or we access it via Owner.
        // CNPCBase has Get_StateCom() (inherited from CCharacterBase? Let's check).
        // Actually we can just implement helper here or in Controller.

        // Helper Logic for Direction
        if (m_Owner)
        {
            auto state = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
            if (state)
            {
                if (dir.x > 0.1f) state->Change_Dir(ACTORDIR::RIGHT);
                else if (dir.x < -0.1f) state->Change_Dir(ACTORDIR::LEFT);
            }
        }

        // Move Force
        // _float speed = m_pController->Get_OwnerCombatStat()->Get_Speed(); // If available
        _float speed = 3.0f; // Default for now

        // CMonsterController has Get_OwnerCombatStat()
        if (m_Controller)
        {
            auto stat = m_Controller->Get_OwnerCombatStat();
            if (stat) speed = stat->Get_Speed();
        }

        m_OwnerRigidbody->Add_Force(dir * speed * 20.f);
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
