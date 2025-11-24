#include "pch.h"
#include "CBossAwake.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"

CBossAwake::CBossAwake(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossAwake::~CBossAwake()
{
}

void CBossAwake::Enter()
{
    CMonsterState::Enter();
}

void CBossAwake::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossAwake::Ready_State()
{
    //m_dwCurStateKey = CBoss::BOSS_STATE::AWAKE;

    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossAwake::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    return _int();
}

void CBossAwake::LateUpdate_State(const _float& timeDelta)
{
}

void CBossAwake::Render_State()
{
    m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::LOOP);
}

CBossAwake* CBossAwake::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossAwake = new CBossAwake(pOwner, pStateMachine);

    if (FAILED(BossAwake->Ready_State()))
    {
        MSG_BOX("BossAwake Create Failed");
        Safe_Release(BossAwake);
        return nullptr;
    }

    return BossAwake;
}

void CBossAwake::Free()
{
}
