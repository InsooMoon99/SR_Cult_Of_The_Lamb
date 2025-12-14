#pragma once
#include "CNPCState.h"

/// @brief NPC가 플레이어를 지나쳐 이동하는 상태
class CNPCState_WalkPast : public CNPCState
{
public:
    explicit CNPCState_WalkPast(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_WalkPast() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_WalkPast* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

protected:
    void Free() override;
};
