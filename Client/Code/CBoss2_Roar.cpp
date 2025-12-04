#include "pch.h"
#include "CBoss2_Roar.h"

#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"

CBoss2_Roar::CBoss2_Roar(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_Roar::~CBoss2_Roar()
{
}

void CBoss2_Roar::Enter()
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

_int CBoss2_Roar::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    auto animator = m_Controller->Get_OwnerAnimator();
    if (animator->Is_AnimationFinished())
    {
        return CBossPhase2::PATTERN::PHASE2_IDLE;
    }

    /*if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CBossPhase2::PATTERN::PHASE2_START;
    }*/

    return CBossPhase2::PATTERN::PHASE2_EYEBALL;
}

void CBoss2_Roar::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Roar::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    return S_OK;
}

CBoss2_Roar* CBoss2_Roar::Create(CGameObject* owner)
{
    auto state = new CBoss2_Roar(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Roar::Free()
{
    
}
