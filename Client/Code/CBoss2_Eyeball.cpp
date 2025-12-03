#include "pch.h"
#include "CBoss2_Eyeball.h"

#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"

CBoss2_Eyeball::CBoss2_Eyeball(CGameObject* owner)
    : m_Controller(nullptr), m_Owner(nullptr), m_Pattern_Cooldown(0)
{
}

CBoss2_Eyeball::~CBoss2_Eyeball()
{
}

void CBoss2_Eyeball::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Roar", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
}

_int CBoss2_Eyeball::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    // 3초 후 Idle 공격으로 전환
    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CBossPhase2::PATTERN::PHASE2_IDLE;
    }

    return CBossPhase2::PATTERN::PHASE2_EYEBALL;
}

void CBoss2_Eyeball::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Eyeball::Ready_State()
{
    m_fMinStateTime = 0.5f;

    return S_OK;
}

CBoss2_Eyeball* CBoss2_Eyeball::Create(CGameObject* owner)
{
    auto state = new CBoss2_Eyeball(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Eyeball::Free()
{
    
}
