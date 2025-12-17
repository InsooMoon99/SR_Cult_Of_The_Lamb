#include "pch.h"
#include "CEligos_Attack_Melee.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CParticleHelper.h"
#include "CPlayer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"

CEligos_Attack_Melee::CEligos_Attack_Melee(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Attack_Melee::~CEligos_Attack_Melee()
{

}

void CEligos_Attack_Melee::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attackcharge", ANIMSTATE::ONCE);

    m_ePhase = PHASE::READY;
}

void CEligos_Attack_Melee::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Attack_Melee::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    m_pOwnerRigidbodyCom = static_cast<CRigidbody*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));
    Find_Player();


    return S_OK;
}

_int CEligos_Attack_Melee::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::ATTACK_MELEE;

    m_fDashTime += timeDelta;

    switch (m_ePhase)
    {

    case PHASE::READY:

        tmpOwnerPos = { m_pOwnerTransformCom->Get_Pos().x,m_pPlayerTransformCom->Get_Pos().y,m_pOwnerTransformCom->Get_Pos().z };
        m_TargetDir = m_pPlayerTransformCom->Get_Pos() - tmpOwnerPos;
        D3DXVec3Normalize(&m_TargetDir, &m_TargetDir);
        // 플레이어 위치 캐싱

        if (m_pOwnerTransformCom->Get_Pos().x < m_pPlayerTransformCom->Get_Pos().x)// 플레이어 오른쪽
        {
            if (m_pOwnerTransformCom->Get_Scale().x > 0)
                m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
        }
        else
        {
            if (m_pOwnerTransformCom->Get_Scale().x < 0)
                m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
        }

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::SHOOT;

        }
        break;

    case PHASE::SHOOT: //근접 공ㄱ겨
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attack", ANIMSTATE::ONCE);

        if (Attack_Melee() == true)
        {
            m_flag = false;
            m_AttackSpawned = false;
            return CEligos::ELIGOSSTATE::BOSSIDLE;

        }


        break;
    }

    return CEligos::ELIGOSSTATE::ATTACK_MELEE;
}

void CEligos_Attack_Melee::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Attack_Melee::Render_State()
{
}

bool CEligos_Attack_Melee::Attack_Melee()
{

    if (m_flag == false)
    {
        m_fDashTime = 0;
        //// 해당 방향으로 순간적으로 이동
        //if (!m_TargetTransform || !m_TransformCom)
        //    return;

        //_vec3 pos = m_TransformCom->Get_Pos();
        //_vec3 targetPos = m_TargetTransform->Get_Pos();
        //
        
        //tmpOwnerPos : 
        //m_TargetDir : 타겟방향

        _vec3 dir = m_TargetDir;
        dir.y = 0.f;

        D3DXVec3Normalize(&dir, &dir);

        //int firstPress =50000;
        //_vec3 moveForce = dir * firstPress;
        //m_pOwnerRigidbodyCom->Add_Force(moveForce);


        _vec3 dashVel = dir * 30.f;
        m_pOwnerRigidbodyCom->Set_Velocity(dashVel);
        m_flag = true;
    }


    if (m_pOwnerAnimatorCom->Get_CurFrame() == 1 && !m_AttackSpawned)
    {
        CAttackBuilder(m_pOwner->Get_GraphicDev(), m_pOwner)
            .SetType<CAttackBox>()
            .Damage(1.f)
            .LifeTime(0.3f)
            .DestroyOnHit(true)
            .InstigatorPos()
            .Offset(m_TargetDir * 1.5f)
            .Knockback(50.f)
            .Scale(_vec3(2.f, 2.f, 2.f))
            .Particle(L"HitEffect_Star", 14, 0.04f)
            .Spawn();

        m_AttackSpawned = true; // 한 프레임에 2개 생성해버려서
    }

    // 실제 오너의 위치가  해당 방향으로 가야함

    if (m_pOwnerAnimatorCom->Is_AnimationFinished()&& m_fDashTime >= 0.5f)
    {
       // m_pOwnerRigidbodyCom->Set_Velocity({0,0,0});
        m_pOwnerRigidbodyCom->Set_Velocity({ 0.f,0.f,0.f });
       //m_pOwnerRigidbodyCom->Set_VelocityLimit({ 10.f,100.f,10.f }); // 원래대로
        return true;
    }
    return false;
}

CEligos_Attack_Melee* CEligos_Attack_Melee::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Attack_Melee(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Attack_Melee::Free()
{

}
