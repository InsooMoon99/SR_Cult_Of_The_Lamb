#include "CBossAwake.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
CBossAwake::CBossAwake()
{
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
    return E_NOTIMPL;
}

_int CBossAwake::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    return _int();
}

void CBossAwake::LateUpdate_State(const _float& timeDelta)
{
}

CBossAwake* CBossAwake::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    return nullptr;
}

void CBossAwake::Free()
{
}
