#include "pch.h"
#include "CBossChainAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"

CBossChainAttack::CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossChainAttack::~CBossChainAttack()
{
}

void CBossChainAttack::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);
    //m_ePhase = PHASE::DOWN; // 이걸 어케 해야하지??
}

void CBossChainAttack::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossChainAttack::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossChainAttack::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSMOVE;

    switch (m_ePhase)
    {
    case PHASE::CIRCLE:

        m_fElapsedTime[(int)PHASE::CIRCLE] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::CIRCLE] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;

        }
        break;


    case PHASE::ROTCROSS_SPREAD: //움직임

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::LOOP);

        m_fElapsedTime[(int)PHASE::ROTCROSS_SPREAD] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::ROTCROSS_SPREAD] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;

    case PHASE::SPREAD://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossMove_up", ANIMSTATE::ONCE);

        m_fElapsedTime[(int)PHASE::SPREAD] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::SPREAD] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::BOSSMOVE;
}

void CBossChainAttack::LateUpdate_State(const _float& timeDelta)
{
}

CBossChainAttack* CBossChainAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossCA = new CBossChainAttack(pOwner, pStateMachine);

    if (FAILED(BossCA->Ready_State()))
    {
        MSG_BOX("BossCA Create Failed");
        Safe_Release(BossCA);
        return nullptr;
    }

    return BossCA;
}

void CBossChainAttack::Free()
{
}
