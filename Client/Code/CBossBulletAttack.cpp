#include "pch.h"
#include "CBossBulletAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"

CBossBulletAttack::CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossBulletAttack::~CBossBulletAttack()
{
}

void CBossBulletAttack::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);
    //m_ePhase = PHASE::DOWN; // 이걸 어케 해야하지??
}

void CBossBulletAttack::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossBulletAttack::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossBulletAttack::Update_State(const _float& timeDelta)
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

void CBossBulletAttack::LateUpdate_State(const _float& timeDelta)
{
    

}

void CBossBulletAttack::Render_State()
{
   // m_pOwnerAnimatorCom->Play_Animation(L"BossAwake", ANIMSTATE::LOOP);
}

CBossBulletAttack* CBossBulletAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossBA = new CBossBulletAttack(pOwner, pStateMachine);

    if (FAILED(BossBA->Ready_State()))
    {
        MSG_BOX("BossAwake Create Failed");
        Safe_Release(BossBA);
        return nullptr;
    }

    return BossBA;
}

void CBossBulletAttack::Free()
{
}
