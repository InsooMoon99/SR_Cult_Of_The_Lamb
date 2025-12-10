#pragma once
#include "CNPCState.h"

class CNPCState_Idle : public CNPCState
{
private:
    explicit CNPCState_Idle(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Idle() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override; // Base does not have Start/Ready pure virtual, but we use it for init

public:
    static CNPCState_Idle* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
