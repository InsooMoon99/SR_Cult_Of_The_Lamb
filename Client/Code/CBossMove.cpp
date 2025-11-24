#include "pch.h"
#include "CBossMove.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
CBossMove::CBossMove(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossMove::~CBossMove()
{
}

void CBossMove::Enter()
{
}

void CBossMove::Exit()
{
}

HRESULT CBossMove::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossMove::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossMove::LateUpdate_State(const _float& timeDelta)
{
}

CBossMove* CBossMove::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossMove::Free()
{
}
