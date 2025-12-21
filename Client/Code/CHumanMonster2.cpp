#include "pch.h"
#include "CHumanMonster2.h"

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
#include "CSoundMgr.h"

CHumanMonster2::CHumanMonster2(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{}

CHumanMonster2::CHumanMonster2(const CHumanMonster2& rhs)
    : CMonsterBase(rhs),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{}

CHumanMonster2::~CHumanMonster2()
{}

HRESULT CHumanMonster2::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(1.5f, 1.3f, 1.2f));

    m_CombatStatCom->Set_MaxHp(200.f);
    m_CombatStatCom->Set_Speed(65.f);

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(0.3f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    // 공격 주기 설정
    m_AttackCooldown = 1.3f;
    m_AttackTimer    = 0.f;

    // 거리 설정
    Set_StoppingDistance(2.5f);
    Set_AttackRange(2.5f);
    Set_ChaseRange(10.f);

    return S_OK;
}

void CHumanMonster2::Initialize_GameObject()
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

_int CHumanMonster2::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (Check_CommonTransitions())
        return exit;

    Update_Direction();

    ACTORSTATE curState = m_StateCom->Get_State();

    switch (curState)
    {
    case ACTORSTATE::IDLE: Update_State_Idle(timeDelta);
        break;
    case ACTORSTATE::RUN: Update_State_Run(timeDelta);
        break;
    case ACTORSTATE::ATTACK: Update_State_Attack(timeDelta);
        break;
    case ACTORSTATE::HIT: Update_State_Hit(timeDelta);
        break;
    case ACTORSTATE::DEAD: Update_State_Dead(timeDelta);
        break;
    }

    return exit;
}

void CHumanMonster2::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CHumanMonster2::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

_bool CHumanMonster2::Check_CommonTransitions()
{
    ACTORSTATE curState = m_StateCom->Get_State();

    if (m_CombatStatCom->Is_Dead())
    {
        if (curState != ACTORSTATE::DEAD)
            m_StateCom->Change_State(ACTORSTATE::DEAD);

        return false;
    }

    if (curState != ACTORSTATE::HIT && m_CombatStatCom->Is_Knockback())
    {
        m_StateCom->Change_State(ACTORSTATE::HIT);

        return true;
    }

    return false;
}

void CHumanMonster2::Update_State_Idle(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    // 공격 가능하고 거리 안에 있으면
    if (Is_InAttackRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
        m_AttackTimer = 0.f;
    }
    // 추적 범위 안에 있으면 RUN
    else if (Is_InChaseRange())
        m_StateCom->Change_State(ACTORSTATE::RUN);
}

void CHumanMonster2::Update_State_Run(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    _float dist = Get_DistanceToTarget();

    // 쿨타임 중
    if (!Can_Attack())
    {
        const _float MAX_RETREAT_DIST = 4.5f;

        if (dist < MAX_RETREAT_DIST)
        {
            _float factor = 1.f - (dist / MAX_RETREAT_DIST); 
            factor = max(factor, 0.f);

            Retreat_FromTarget(timeDelta, m_CombatStatCom->Get_Speed() * 0.785f * factor);
        }
        // 멀면 그냥 대기

        return;
    }

    if (!Is_InAttackRange())
        Chase_Target(timeDelta, m_CombatStatCom->Get_Speed());

    // 공격 가능
    if (Is_InAttackRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
        m_AttackTimer = 0.f;
    }

    // 추적 범위 밖
    else if (Is_OutofChaseRange())
        m_StateCom->Change_State(ACTORSTATE::IDLE);
}

void CHumanMonster2::Update_State_Attack(const _float timeDelta)
{
    if (m_AnimatorCom->Get_CurFrame() == 3 && !m_AttackSpawned)
    {
        CAttackBuilder(m_GraphicDev, this)
            .SetType<CAttackBox>()
            .Damage(1.f)
            .LifeTime(0.2f)
            .DestroyOnHit(true)
            .InstigatorPos()
            .CollisionLayer(COLLISION_LAYERTYPE::ENEMY)
            .Offset(m_TargetDir * 0.6f)
            .Knockback(50.f)
            .Scale(_vec3(1.f, 1.f, 1.f))
            .Particle(L"HitEffect_Star", 14, 0.04f)
            .Spawn();

        m_AttackSpawned = true; // 한 프레임에 2개 생성해버려서
    }

    if (m_AnimatorCom->Is_AnimationFinished())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
        m_AttackSpawned = false;
    }
}

void CHumanMonster2::Update_State_Hit(const _float timeDelta)
{
    if (!m_HitSoundPlayed)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Enemy/Humanoid_GetHit.wav", SOUNDTYPE::SOUND_HUMANOID, 0.5f);
        m_HitSoundPlayed = true;
    }

    // 애니메이션이 끝나고, 넉백이 다 밀렸을 때
    if (m_AnimatorCom->Is_AnimationFinished() && !m_CombatStatCom->Is_Knockback())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
        m_HitSoundPlayed = false;
    }
}

void CHumanMonster2::Update_State_Dead(const _float timeDelta)
{
    // CombatStat에서 Dead 상태처리 진행중 Dead 애니메이션이 없다. 밀려나면서, 뼛가루 소환?
    if (m_StateCom->Get_State() == ACTORSTATE::DEAD)
    {
        Trigger_OnDeath();
        Mark_As_PendingKill();
    }
}

void CHumanMonster2::Update_Direction()
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    ACTORSTATE curState = m_StateCom->Get_State();

    // Idle, Run일 떄에만 방향 전환 허용
    bool canChangeDir = false;
    canChangeDir |= curState == ACTORSTATE::IDLE;
    canChangeDir |= curState == ACTORSTATE::RUN;

    if (!canChangeDir)
        return;

    if (m_TargetDir.x > 0.f)
        m_StateCom->Change_Dir(ACTORDIR::RIGHT);
    else
        m_StateCom->Change_Dir(ACTORDIR::LEFT);
}

void CHumanMonster2::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Human2_Idle", 34, 0.02f,
        _vec2(0.63f, 0.69f),
        _vec2(0.12f, -0.05f), _vec2(-0.14f, -0.05f));

    m_AnimatorCom->Create_Animation(L"Human2_Attack", 56, 0.02f,
        _vec2(1.07f, 1.09f),
_vec2(0.08f, -0.41f), _vec2(-0.08f, -0.41f));

    m_AnimatorCom->Create_Animation(L"Human2_Hit", 48, 0.02f,
        _vec2(0.66f, 0.91f),
        _vec2(0.14f, 0.04f), _vec2(-0.14f, 0.04f));

    m_AnimatorCom->Create_Animation(L"Human2_Run", 29, 0.02f,
        _vec2(1.f, 1.f),
        _vec2(0.f, -0.42f));

    m_AnimatorCom->Create_Animation(L"Human2_RunUp", 34, 0.02f,
        _vec2(1.f, 1.f),
        _vec2(0.f, -0.42f));

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Human2_Idle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"Human2_Attack", ANIMSTATE::ONCE);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"Human2_Run", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::HIT, L"Human2_Hit", ANIMSTATE::ONCE);
}

void CHumanMonster2::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);


}

void CHumanMonster2::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

void CHumanMonster2::Chase_Target(const float& timeDelta, _float speed)
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos       = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    //m_TransformCom->Chase_Target(&targetPos, timeDelta, speed);

    _vec3 dir = targetPos - pos;
    dir.y     = 0.f;

    D3DXVec3Normalize(&dir, &dir);

    _vec3 moveForce = dir * m_CombatStatCom->Get_Speed();
    m_RigidbodyCom->Add_Force(moveForce);
}

void CHumanMonster2::Retreat_FromTarget(const _float& timeDelta, _float speed)
{
    CMonsterBase::Retreat_FromTarget(timeDelta, speed);

    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();

    _vec3 dir = pos - targetPos;
    dir.y = 0.f;

    D3DXVec3Normalize(&dir, &dir);

    m_TransformCom->Move_Pos(dir, timeDelta, speed * 0.05f);

    /*_vec3 moveForce = dir * speed;

    m_RigidbodyCom->Add_Force(moveForce);*/

}

HRESULT CHumanMonster2::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_HUMAN_MONSTER2);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_HUMAN_MONSTER2, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_HUMAN_MONSTER2);

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    return S_OK;
}

CHumanMonster2* CHumanMonster2::Create(DEVICE graphicDev)
{
    auto humanmonster = new CHumanMonster2(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("HumanMonster Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CHumanMonster2::Free()
{
    CMonsterBase::Free();
}
