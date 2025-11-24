#include "pch.h"
#include "CBossDead.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
HRESULT CBossDead::Ready_State()
{
    return E_NOTIMPL;
}

_int CBossDead::Update_State(const _float& timeDelta)
{
    return _int();
}

void CBossDead::LateUpdate_State(const _float& timeDelta)
{
}

inline void CBossDead::Render_State() {}

CBossDead* CBossDead::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossDead::Free()
{
}

CBossDead::CBossDead(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
}

CBossDead::~CBossDead()
{
}

void CBossDead::Enter()
{
}

void CBossDead::Exit()
{
}
