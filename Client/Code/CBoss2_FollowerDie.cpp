#include "pch.h"
#include "CBoss2_FollowerDie.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CTimerMgr.h"
#include "CEyeball.h"
#include "CStateMachine.h"
#include "CSoundMgr.h"

CBoss2_FollowerDie::CBoss2_FollowerDie(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_FollowerDie::~CBoss2_FollowerDie()
{
}

void CBoss2_FollowerDie::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Follower_Die", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
}

_int CBoss2_FollowerDie::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {

    }

    return CBossPhase2::PATTERN::PHASE2_FOLLOWER_DIE;
}

void CBoss2_FollowerDie::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_FollowerDie::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    m_ChainAnim = ROAR;

    return S_OK;
}

CBoss2_FollowerDie* CBoss2_FollowerDie::Create(CGameObject* owner)
{
    auto state = new CBoss2_FollowerDie(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_FollowerDie::Free()
{
    
}
