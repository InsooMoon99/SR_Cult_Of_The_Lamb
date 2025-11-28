#include "pch.h"
#include "CBossBulletAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CRandomMgr.h"

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
    // TODO:: 지금 이게 안들어오거든 바로 상태안의 애니로 감 어칼까

    m_ePhase = PHASE::CIRCLE; // 이걸 어케 해야하지??
    int RandPhase = CRandomMgr::GetInstance()->Get_Int_Random(0, ((int)PHASE::PHASE_END) - 1); // 페이즈 랜덤
    m_ePhase = (PHASE)RandPhase;


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
        return CBossPhase1::BOSSPHASE1::BULLETATTACK;

    switch (m_ePhase)
    {
    case PHASE::CIRCLE:

        m_fElapsedTime+= timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime= 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;

        }
        break;


    case PHASE::ROTCROSS_SPREAD: //움직임

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::LOOP);

        m_fElapsedTime += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;

    case PHASE::SPREAD://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);

        m_fElapsedTime += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::BULLETATTACK;
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
