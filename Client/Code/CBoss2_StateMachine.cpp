#include "pch.h"
#include "CBoss2_StateMachine.h"

CBoss2_StateMachine::CBoss2_StateMachine(CMonsterBase* owner)
{

}

CBoss2_StateMachine::~CBoss2_StateMachine()
{

}

HRESULT CBoss2_StateMachine::Ready_StateMachine()
{


    return S_OK;
}

_int CBoss2_StateMachine::Update_StateMachine(const _float& timeDelta)
{


    return 0;
}

void CBoss2_StateMachine::Change_State(_uint dwStateKey)
{

}

CBoss2_StateMachine* CBoss2_StateMachine::Create(CMonsterBase* owner)
{
    CBoss2_StateMachine* stateMachine = new CBoss2_StateMachine(owner);

    if (FAILED(stateMachine->Ready_StateMachine()))
    {
        Safe_Release(stateMachine);
        MSG_BOX("Boss2 State Machine Create Failed");
        return nullptr;
    }

    return stateMachine;
}

void CBoss2_StateMachine::Free()
{
    CStateMachine::Free();

}
