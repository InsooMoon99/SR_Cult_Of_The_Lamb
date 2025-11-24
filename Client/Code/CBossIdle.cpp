#include "pch.h"
#include "CBossIdle.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
CBossIdle::CBossIdle(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossIdle::~CBossIdle()
{
}

void CBossIdle::Enter()
{
}

void CBossIdle::Exit()
{
}

HRESULT CBossIdle::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossIdle::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossIdle::LateUpdate_State(const _float& timeDelta)
{
}

CBossIdle* CBossIdle::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossIdle::Free()
{
}
