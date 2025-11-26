#include "pch.h"
#include "CBossIdle.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CTransform.h"

CBossIdle::CBossIdle(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossIdle::~CBossIdle()
{
}

void CBossIdle::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::LOOP);

}

void CBossIdle::Exit()
{
    CMonsterState::Exit();
}

HRESULT CBossIdle::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossIdle::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSIDLE;

    // 여기서는 애니는 이미 Idle LOOP 중이라고 가정
    // (Enter에서 재생)

    //if (패턴쿨타임끝)
    //{
    //    if (랜덤 < 0.3f) return BOSS_STATE::CHAIN_PATTERN_1;
    //    if (랜덤 < 0.6f) return BOSS_STATE::CHAIN_PATTERN_2;
    //    return BOSS_STATE::BULLET_PATTERN_1;
    //}

    return CBossPhase1::BOSSPHASE1::BOSSIDLE;
}

void CBossIdle::LateUpdate_State(const _float& timeDelta)
{
}

CBossIdle* CBossIdle::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossIdle = new CBossIdle(pOwner, pStateMachine);

    if (FAILED(BossIdle->Ready_State()))
    {
        MSG_BOX("BossIdle Create Failed");
        Safe_Release(BossIdle);
        return nullptr;
    }

    return BossIdle;
}

void CBossIdle::Free()
{
}
