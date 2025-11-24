#include "pch.h"
#include "CBossBulletAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
CBossBulletAttack::CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossBulletAttack::~CBossBulletAttack()
{
}

void CBossBulletAttack::Enter()
{
}

void CBossBulletAttack::Exit()
{
}

HRESULT CBossBulletAttack::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossBulletAttack::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossBulletAttack::LateUpdate_State(const _float& timeDelta)
{
}

CBossBulletAttack* CBossBulletAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossBulletAttack::Free()
{
}
