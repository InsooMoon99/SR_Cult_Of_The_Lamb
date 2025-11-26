#include "pch.h"
#include "CBossMove.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"

CBossMove::CBossMove(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossMove::~CBossMove()
{
}

void CBossMove::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossMove_Down", ANIMSTATE::ONCE);
    m_ePhase = PHASE::DOWN;
}

void CBossMove::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossMove::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossMove::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSMOVE;

    switch (m_ePhase)
    {
    case PHASE::DOWN:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            m_ePhase = PHASE::WAIT;
        break;

    case PHASE::WAIT: //움직임

        //m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);
        m_fCurStateElapsedTime += timeDelta;

        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_ePhase = PHASE::UP;
        }
        break;

    case PHASE::UP://움직임 + idle
  
        m_pOwnerAnimatorCom->Play_Animation(L"BossMove_up", ANIMSTATE::ONCE);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            return CBossPhase1::BOSSPHASE1::BULLETATTACK;// 이거 아님 공격상태전환 아이들로 
        break;
    }



    return CBossPhase1::BOSSPHASE1::BOSSMOVE;
}

void CBossMove::LateUpdate_State(const _float& timeDelta)
{
}

CBossMove* CBossMove::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossMove = new CBossMove(pOwner, pStateMachine);

    if (FAILED(BossMove->Ready_State()))
    {
        MSG_BOX("BossMove Create Failed");
        Safe_Release(BossMove);
        return nullptr;
    }

    return BossMove;
}

void CBossMove::Free()
{
}
