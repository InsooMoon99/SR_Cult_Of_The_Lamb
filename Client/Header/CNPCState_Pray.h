#pragma once
#include "CNPCState.h"

/// @brief NPC의 기도(Pray) 상태
class CNPCState_Pray : public CNPCState
{
private:
    explicit CNPCState_Pray(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Pray() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Pray* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
