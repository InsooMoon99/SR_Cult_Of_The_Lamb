#include "pch.h"
#include "CBatMonster.h"
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

CBatMonster::CBatMonster(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{}

CBatMonster::CBatMonster(const CBatMonster& rhs)
    : CMonsterBase(rhs),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{}

CBatMonster::~CBatMonster()
{}

HRESULT CBatMonster::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(1.5f, 1.3f, 1.2f));

    m_CombatStatCom->Set_MaxHp(150.f);
    m_CombatStatCom->Set_Speed(65.f);
    m_CombatStatCom->Set_HitState(false); // Hit State로 변경할건지

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(false);
    m_RigidbodyCom->Set_UseFriction(false);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(0.3f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    // 공격 주기 설정
    m_AttackCooldown = 2.0f + (rand() % 150) / 100.f; 
    m_AttackTimer = (rand() % 100) / 100.f * m_AttackCooldown;  

    // 거리 설정
    Set_StoppingDistance(5.5f);
    Set_AttackRange(5.5f);
    Set_ChaseRange(30.f);

    return S_OK;
}

void CBatMonster::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
                                                  LAYERTYPE::GAMELOGIC,
                                                  OBJTYPE::PLAYER,
                                                  COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();

    Setup_SpawnDeadParticle();
}

_int CBatMonster::Update_GameObject(const _float& timeDelta)
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

void CBatMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CBatMonster::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

_bool CBatMonster::Check_CommonTransitions()
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

void CBatMonster::Setup_SpawnDeadParticle()
{
    m_CombatStatCom->Set_OnDeathEvent([this]()
        {
            CParticleHelper::Spawn_StaticBurstParticle(
                m_TransformCom->Get_Pos(),
                COMPONENTTYPE::TEX_MONSTER_BAT_HIT_PARTICLE, 
                8,                                  
                0.15f,
                0.2f,
                3,
                12.f,
                m_TransformCom->Get_Pos().y - 0.5f
            );
        });
}

void CBatMonster::Update_State_Idle(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    Wandering_Around(timeDelta);

    if (Is_InChaseRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::RUN);
        return;
    }

}

void CBatMonster::Update_State_Run(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    if (!m_TargetTransform)
        return;

    if (!Is_InChaseRange())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
        return;
    }

    if (Is_InAttackRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
        m_AttackTimer = 0.f;
        m_AttackCooldown = 2.0f + (rand() % 150) / 100.f;  
        return;
    }

    Chase_Target(timeDelta, m_CombatStatCom->Get_Speed());
}

void CBatMonster::Update_State_Attack(const _float timeDelta)
{
    wstring curAnim = m_AnimatorCom->Get_CurKey();

    if (curAnim == L"Bat_Charge")
    {
        if (m_AnimatorCom->Get_CurFrame() == 0)
        {
            m_ChargeDir = m_TargetDir;  
            m_ChargedDistance = 0.f;
            m_ChargeDistance = 1.25f;     
        }

        if (m_AnimatorCom->Is_AnimationFinished())
        {
            m_AnimatorCom->Play_Animation(L"Bat_Attack", ANIMSTATE::ONCE);

            CAttackBuilder(m_GraphicDev, this)
                .SetType<CAttackBox>()
                .Damage(1.f)
                .Track(m_AttackBox)
                .LifeTime(0.8f)
                .DestroyOnHit(false)
                .CollisionLayer(COLLISION_LAYERTYPE::ATTACK_MONSTER)
                .InstigatorPos()  
                .Knockback(50.f)
                .Scale(_vec3(0.8f, 0.8f, 0.8f))
                .Spawn();
        }
    }

    else if (curAnim == L"Bat_Attack")
    {
        if (m_ChargedDistance < m_ChargeDistance)
        {
            _float speed = m_CombatStatCom->Get_Speed() * 0.07f;
            _float moveAmount = speed * timeDelta;

            if (m_AttackBox)
            {
                _vec3 batPos = m_TransformCom->Get_Pos();
                m_AttackBox->Get_Transform()->Set_Pos(batPos);
            }

            m_TransformCom->Move_Pos(m_ChargeDir, timeDelta, speed);
            m_ChargedDistance += moveAmount;
        }

        if (m_AnimatorCom->Is_AnimationFinished())
        {
            m_AttackBox = nullptr;

            if (m_TargetTransform)
            {
                _vec3 playerPos = m_TargetTransform->Get_Pos();
                _vec3 curPos = m_TransformCom->Get_Pos();
                _vec3 toPlayer = curPos - playerPos;

                m_WanderAngle = atan2f(toPlayer.z, toPlayer.x);  
            }


            m_StateCom->Change_State(ACTORSTATE::IDLE);

            m_StateCom->Change_State(ACTORSTATE::IDLE);
            m_AttackCooldown = 2.0f + (rand() % 150) / 100.f;
        }
    }
}

void CBatMonster::Update_State_Hit(const _float timeDelta)
{
    // 애니메이션이 끝나고, 넉백이 다 밀렸을 때
    if (m_AnimatorCom->Is_AnimationFinished() && !m_CombatStatCom->Is_Knockback())
        m_StateCom->Change_State(ACTORSTATE::IDLE);
}

void CBatMonster::Update_State_Dead(const _float timeDelta)
{
    // CombatStat에서 Dead 상태처리 진행중 Dead 애니메이션이 없다. 밀려나면서, 뼛가루 소환?
    if (m_StateCom->Get_State() == ACTORSTATE::DEAD)
    {
        Trigger_OnDeath();
        Mark_As_PendingKill();
    }
}

void CBatMonster::Update_Direction()
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    ACTORSTATE curState = m_StateCom->Get_State();

    // Idle, Run일 떄에만 방향 전환 허용
    _bool canChangeDir = false;
    canChangeDir |= curState == ACTORSTATE::IDLE;
    canChangeDir |= curState == ACTORSTATE::RUN;

    if (!canChangeDir)
        return;

    const _float DEAD_ZONE = 0.05f;

    if (m_TargetDir.x > DEAD_ZONE)
        m_StateCom->Change_Dir(ACTORDIR::RIGHT);

    else if (m_TargetDir.x < -DEAD_ZONE)
        m_StateCom->Change_Dir(ACTORDIR::LEFT);
}

void CBatMonster::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Bat_Idle", 34, 0.02f,
        _vec2(0.84f, 0.71f),
        _vec2(0.f, 0.62f), _vec2(0.f, 0.62f));

    m_AnimatorCom->Create_Animation(L"Bat_Charge", 59, 0.02f,
        _vec2(1.f, 0.83f),
        _vec2(0.f, 0.28f), _vec2(0.f, 0.28f));

    m_AnimatorCom->Create_Animation(L"Bat_Attack", 32, 0.02f,
        _vec2(1.21f, 1.f),
        _vec2(0.f, 0.28f), _vec2(0.f, 0.28f));

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Bat_Idle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"Bat_Charge", ANIMSTATE::ONCE);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"Bat_Idle", ANIMSTATE::LOOP);
}

void CBatMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

}

void CBatMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

void CBatMonster::Chase_Target(const float& timeDelta, _float speed)
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();

    _vec3 dir = targetPos - pos;
    dir.y = 0.f;

    const _float dist = D3DXVec3Length(&dir);
    if (dist < 0.0001f)
        return;

    D3DXVec3Normalize(&dir, &dir);

    if (dist <= m_StoppingDistance)
        return;

    m_TransformCom->Move_Pos(dir, timeDelta, speed * 0.08f);
}

void CBatMonster::Retreat_FromTarget(const _float& timeDelta, _float speed)
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

}

HRESULT CBatMonster::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BAT_MONSTER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BAT_MONSTER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BAT_MONSTER);

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    return S_OK;
}

void CBatMonster::Wandering_Around(const _float timeDelta)
{
    m_WanderAngle += m_WanderSpeed * timeDelta;

    if (m_WanderAngle > D3DX_PI * 2.f)
        m_WanderAngle -= D3DX_PI * 2.f;

    _vec3 moveDir;
    moveDir.x = cosf(m_WanderAngle) * 0.2f;  
    moveDir.z = sinf(m_WanderAngle * 2.f) * 0.8f;  
    moveDir.y = 0.f;

    m_TransformCom->Move_Pos(moveDir, timeDelta, m_CombatStatCom->Get_Speed() * 0.07f);
}

CBatMonster* CBatMonster::Create(DEVICE graphicDev)
{
    auto humanmonster = new CBatMonster(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("HumanMonster Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CBatMonster::Free()
{
    CMonsterBase::Free();
}
