#pragma once
#include "CNPCState.h"

class CNPCState_Run : public CNPCState
{
private:
    explicit CNPCState_Run(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Run() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Run* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
