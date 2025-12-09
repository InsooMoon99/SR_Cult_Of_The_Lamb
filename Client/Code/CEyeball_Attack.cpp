#include "pch.h"
#include "CEyeball_Attack.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CBoss2_Bullet.h"
#include "CEyeball.h"
#include "CAttackBuilder.h"
#include "CMonsterController.h"
#include "CProjectile.h"
#include "CTimerMgr.h"

CEyeball_Attack::CEyeball_Attack(CMonsterBase* owner)
    : m_Owner(owner),
      m_Pattern_Cooldown(0),
      m_LerpSpeed(0),
      m_EyeballAnim(Eyeball_Anim::EYEBALL_ATTACK_END),
      m_HasFiredBullet(false), m_AttackCount(0), m_IsFinished(false),
      m_Bullet_AttackType(Attack_Type::ATTACK_TYPE_END)
{
}

CEyeball_Attack::~CEyeball_Attack()
{
}

void CEyeball_Attack::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();

        if (animator)
        {
            animator->Play_Animation(L"Eyeball_Anti", ANIMSTATE::ONCE);
        }
    }

    CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        m_StartPos = transform->Get_Pos();  

        m_TargetPos = m_StartPos;
        m_TargetPos.y = 1.f;  
    }

    m_fCurStateElapsedTime = 0.f;

    m_EyeballAnim = Eyeball_Anim::ATTACK_ANTI;
    m_Bullet_AttackType = Attack_Type::DISPERSION;

    Find_Player(); // 플레이어 찾기
}

_int CEyeball_Attack::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    CTransform* transform = m_Controller->Get_OwnerTransform();
    auto animator = m_Controller->Get_OwnerAnimator();

    // 끝나면 Idle로
    if (m_IsFinished)
        return CEyeball::PATTERN::EYEBALL_IDLE;

    switch (m_EyeballAnim)
    {
    case Eyeball_Anim::ATTACK_ANTI:
        Play_Anti();
        break;

    case Eyeball_Anim::ATTACK_LOOP:
        Play_Loop();
        break;

    case Eyeball_Anim::ATTACK_END:
        Play_End();
        break;

    }

    /*if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_MOVE; 
    }*/

    return CEyeball::PATTERN::EYEBALL_ATTACK;
}

void CEyeball_Attack::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Attack::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

    m_LerpSpeed = 0.05f;
    m_EyeballAnim = Eyeball_Anim::ATTACK_ANTI;

    m_HasFiredBullet = false;
    m_AttackCount = 0;
    m_IsFinished = false;
    
    return S_OK;
}

void CEyeball_Attack::Play_Anti()
{
    // 플레이어 반대 방향으로 이동 시키기?
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator)
    {
        if (animator->Is_AnimationFinished())
        {
            m_EyeballAnim = Eyeball_Anim::ATTACK_LOOP;
            animator->Play_Animation(L"Eyeball_Attack_Loop", ANIMSTATE::LOOP);
        }
        else
        {
            _vec3 dir = (m_Owner->Get_Transform()->Get_Pos() - m_pPlayerTransformCom->Get_Pos());
            dir.y = 0;
            D3DXVec3Normalize(&dir, &dir);

            _float delta = CTimerMgr::GetInstance()->Get_TimeDelta();
            m_Owner->Get_Transform()->Move_Pos(dir, delta, 2.f);
        }

    }
}

void CEyeball_Attack::Play_Loop()
{
    // 공격
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator)
    {
        _int curFrame = animator->Get_CurFrame();

        // 20 프레임에 한번만 쏘기
        if (curFrame >= 20 && !m_HasFiredBullet && m_AttackCount < 2)
        {
            m_AttackCount++;

            Spawn_BulletPattern();
            m_HasFiredBullet = true;
        }

        if (curFrame >= 40)
        {
            m_HasFiredBullet = false;
        }

        if (m_AttackCount >= 2)
        {
            m_EyeballAnim = Eyeball_Anim::ATTACK_END;
            animator->Play_Animation(L"Eyeball_Attack_End", ANIMSTATE::ONCE);
        }

    }

}

void CEyeball_Attack::Play_End()
{
    m_HasFiredBullet = false;

    // 공격 끝 -> Move로 상태 전환
    auto animator = m_Controller->Get_OwnerAnimator();

    // 일단 ㅌㅌ
    if (animator)
    {
        if (animator->Is_AnimationFinished())
        {
            m_IsFinished = true;

            return;
        }

        _vec3 dir = (m_Owner->Get_Transform()->Get_Pos() - m_pPlayerTransformCom->Get_Pos());
        dir.y = 0;
        D3DXVec3Normalize(&dir, &dir);

        _float delta = CTimerMgr::GetInstance()->Get_TimeDelta();
        m_Owner->Get_Transform()->Move_Pos(dir, delta, 1.f);
    }

}

void CEyeball_Attack::Spawn_BulletPattern()
{
    switch (m_Bullet_AttackType)
    {
    case Attack_Type::DISPERSION:
        Bullet_Dispresion();

    case Attack_Type::CIRCLE:
        Bullet_Circle();
        break;

    case Attack_Type::BURST:
        Bullet_Burst();
        break;
    }
}

void CEyeball_Attack::Bullet_Dispresion()
{
    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    // 원형 패턴 8개 탄환 360도 처리
    _int bulletCount = 8;
    _float angleStep = 360.f / static_cast<_float>(bulletCount);

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float angle = D3DXToRadian(angleStep * i);
        _vec3 direction(cosf(angle), 0.f, sinf(angle));

        CBoss2_Bullet* bullet = nullptr;

        DEVICE graphicDev = m_Owner->Get_GraphicDev();

        CAttackBuilder(graphicDev, m_Owner)
            .SetType<CBoss2_Bullet>()
            .Track(bullet)
            .Position(eyeballPos)
            .Scale(0.2f)
            .Damage(1.f)
            .Knockback(500.f)
            .LifeTime(5.f)
            .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
            .DestroyOnHit(true)
            .Particle(L"HitEffect_Star", 14)
            .Spawn();

        if (bullet)
        {
            auto sphereCol = static_cast<CSphereCollider*>(
                bullet->Get_Component(ID_DYNAMIC, COMPONENTTYPE::SPHERE_COLL));

            if (sphereCol)
                sphereCol->Set_Radius(0.5f);

            CProjectile* projectileCom = dynamic_cast<CProjectile*>(
                bullet->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));

            if (projectileCom)
            {
                BulletDesc desc;
                desc.ProjectileOwner = m_Owner;
                desc.moveType = PROJ_TYPE::LINEAR;
                desc.Speed = 15.f;
                desc.Direction = direction;

                projectileCom->SetMoveDesc(desc);
            }

            bullet->Start_Shoot();
        }
       
    }
}

void CEyeball_Attack::Bullet_Circle()
{
}

void CEyeball_Attack::Bullet_Burst()
{
}

CEyeball_Attack* CEyeball_Attack::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Attack(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Attack Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Attack::Free()
{
    
}
