#include "pch.h"
#include "CBossChainAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"

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
        return CBossPhase1::BOSSPHASE1::CHAINATTACK;

    switch (m_ePhase)
    {
    case PHASE::PLAYERLINE:

        m_fElapsedTime[(int)PHASE::PLAYERLINE] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::PLAYERLINE] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;

        }
        break;


    case PHASE::CROSS: //움직임

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE); 

        m_fElapsedTime[(int)PHASE::CROSS] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::CROSS] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;

    case PHASE::RANDCHAIN://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);

        m_fElapsedTime[(int)PHASE::RANDCHAIN] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::RANDCHAIN] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::CHAINATTACK;
}

void CBossChainAttack::LateUpdate_State(const _float& timeDelta)
{
}

void CBossChainAttack::Render_State()
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
