#include "pch.h"
#include "CBossChainAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"

CBossChainAttack::CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossChainAttack::~CBossChainAttack()
{
}

void CBossChainAttack::Enter()
{
}

void CBossChainAttack::Exit()
{
}

HRESULT CBossChainAttack::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossChainAttack::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossChainAttack::LateUpdate_State(const _float& timeDelta)
{
}

CBossChainAttack* CBossChainAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossChainAttack::Free()
{
}
