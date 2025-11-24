#include "pch.h"
#include "CBossGroggy.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
CBossGroggy::CBossGroggy(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossGroggy::~CBossGroggy()
{
}

void CBossGroggy::Enter()
{
}

void CBossGroggy::Exit()
{
}

HRESULT CBossGroggy::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossGroggy::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossGroggy::LateUpdate_State(const _float& timeDelta)
{
}

CBossGroggy* CBossGroggy::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossGroggy::Free()
{
}
