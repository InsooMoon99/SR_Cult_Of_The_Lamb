#include "pch.h"
#include "CEyeball_Attack.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CMonsterController.h"

CEyeball_Attack::CEyeball_Attack(CMonsterBase* owner)
    : m_Owner(owner),
    m_Pattern_Cooldown(0),
    m_LerpSpeed(0),
    m_EyeballAnim(Eyeball_Anim::EYEBALL_ATTACK_END),
    m_HasFiredBullet(false)
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

    Find_Player(); // 플레이어 찾기
}

_int CEyeball_Attack::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    CTransform* transform = m_Controller->Get_OwnerTransform();
    auto animator = m_Controller->Get_OwnerAnimator();

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
    
    return S_OK;
}

void CEyeball_Attack::Play_Anti()
{
    // 플레이어 반대 방향으로 이동
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        m_EyeballAnim = Eyeball_Anim::ATTACK_LOOP;
        animator->Play_Animation(L"Eyeball_Attack_Loop", ANIMSTATE::LOOP);
    }
}

void CEyeball_Attack::Play_Loop()
{
    // 공격
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        //m_EyeballAnim = Eyeball_Anim::ATTACK_END;
        //animator->Play_Animation(L"Eyeball_Attack_End", ANIMSTATE::ONCE);

        _int curFrame = animator->Get_CurFrame();

        if (curFrame == 20)
        {
            Spawn_BulletPattern();
            m_HasFiredBullet = true;
        }

    }

}

void CEyeball_Attack::Play_End()
{
    // 공격 끝 -> Move로 상태 전환
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {

    }

}

void CEyeball_Attack::Spawn_BulletPattern()
{
    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    // 원형 패턴 8개 탄환 360도 처리
    _int bulletCount = 8;
    _float angleStep = 360.f / static_cast<_float>(bulletCount);

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float angle = D3DXToRadian(angleStep * i);
        _vec3 direction(cosf(angle), 0.f, sinf(angle));

        CBossBullet* bullet = CBossBullet::Create
    }
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
