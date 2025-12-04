#include "pch.h"
#include "CHumanMonster.h"
#include "CCombatStat.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRigidbody.h"

CHumanMonster::CHumanMonster(DEVICE graphicDev)
    : CMonsterBase(graphicDev), m_RigidbodyCom(nullptr)
{
}

CHumanMonster::CHumanMonster(const CHumanMonster& rhs)
    : CMonsterBase(rhs), m_RigidbodyCom(nullptr)
{
}

CHumanMonster::~CHumanMonster()
{}

HRESULT CHumanMonster::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(1.2f, 1.2f, 1.2f));

    m_CombatStatCom->Set_MaxHp(150.f);
    m_CombatStatCom->Set_Hp(150.f);
    m_CombatStatCom->Set_Speed(65.f);

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(0.3f);

    return S_OK;
}

void CHumanMonster::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            LAYERTYPE::GAMELOGIC,
            OBJTYPE::PLAYER,
            COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();
}

_int CHumanMonster::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (Check_CommonTransitions())
        return exit;

    // TODO : 상태 전환 시마다 다음 행동으로 넘어갈 수 있는 딜레이 시간 추가하기
    Update_Direction();

    ACTORSTATE curState = m_StateCom->Get_State();

    switch (curState)
    {
    case ACTORSTATE::IDLE:      Update_State_Idle(timeDelta);  break;
    case ACTORSTATE::RUN:       Update_State_Run(timeDelta); break;
    case ACTORSTATE::ATTACK:    Update_State_Attack(timeDelta); break;
    case ACTORSTATE::HIT:       Update_State_Hit(timeDelta); break;
    case ACTORSTATE::DEAD:      Update_State_Dead(timeDelta); break;
    }

    return exit;
}

void CHumanMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

    
}

void CHumanMonster::Render_GameObject()
{
    CMonsterBase::Render_GameObject();


}

_bool CHumanMonster::Check_CommonTransitions()
{
    ACTORSTATE curState = m_StateCom->Get_State();

    if (curState != ACTORSTATE::HIT && m_CombatStatCom->Is_Knockback())
    {
        m_StateCom->Change_State(ACTORSTATE::HIT);

        return true;
    }

    return false;
}

void CHumanMonster::Update_State_Idle(const _float timeDelta)
{
    _float distance = Get_DistanceToTarget();

    if (2.5f < distance && distance < 10.f)
    {
        m_StateCom->Change_State(ACTORSTATE::RUN);
    }
}

void CHumanMonster::Update_State_Run(const _float timeDelta)
{
    // 타겟 추적
    Chase_Target(timeDelta, m_CombatStatCom->Get_Speed());

    _float distance = Get_DistanceToTarget();

    if (distance <= 2.5f)
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
    }
    else if (distance >= 10.f)
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
    }
}

void CHumanMonster::Update_State_Attack(const _float timeDelta)
{
    if (m_AnimatorCom->Get_CurFrame() == 30)
    {
        // TODO AttackBuilder로 공격 충돌체 소환
    }

    if (m_AnimatorCom->Is_AnimationFinished())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
    }
}

void CHumanMonster::Update_State_Hit(const _float timeDelta)
{
    // 애니메이션이 끝나고, 넉백이 다 밀렸을 때
    if (m_AnimatorCom->Is_AnimationFinished() && !m_CombatStatCom->Is_Knockback())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
    }
}

void CHumanMonster::Update_State_Dead(const _float timeDelta)
{
    // CombatStat에서 Dead 상태처리 진행중 Dead 애니메이션이 없다. 밀려나면서, 뼛가루 소환?
    if (m_StateCom->Get_State() == ACTORSTATE::DEAD)
    {
        Mark_As_PendingKill();
    }
}

void CHumanMonster::Update_Direction()
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 dir = targetPos - pos;

    if (dir.x > 0.f)
        m_StateCom->Change_Dir(ACTORDIR::RIGHT); 
    else
        m_StateCom->Change_Dir(ACTORDIR::LEFT);  

}

void CHumanMonster::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"HumanMonsterIdle", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"HumanMonsterAttack", 65, 0.02f);
    m_AnimatorCom->Create_Animation(L"HumanMonsterHurt", 28, 0.02f);

    // Run
    m_AnimatorCom->Create_Animation(L"HumanMonsterRun", 18, 0.02f);

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"HumanMonsterIdle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"HumanMonsterRun", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::HIT, L"HumanMonsterHurt", ANIMSTATE::ONCE);
    m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"HumanMonsterAttack", ANIMSTATE::ONCE);

}

void CHumanMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

}

void CHumanMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

void CHumanMonster::Chase_Target(const float& timeDelta, _float speed)
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    //m_TransformCom->Chase_Target(&targetPos, timeDelta, speed);

    _vec3 dir = targetPos - pos;
    dir.y = 0.f;

    D3DXVec3Normalize(&dir, &dir);

    _vec3 moveForce = dir * m_CombatStatCom->Get_Speed();
    m_RigidbodyCom->Add_Force(moveForce);
}

HRESULT CHumanMonster::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_HUMAN_MONSTER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_HUMAN_MONSTER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_HUMAN_MONSTER);

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    return S_OK;
}

CHumanMonster* CHumanMonster::Create(DEVICE graphicDev)
{
    auto humanmonster = new CHumanMonster(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("HumanMonster Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CHumanMonster::Free()
{
    CMonsterBase::Free();
}
