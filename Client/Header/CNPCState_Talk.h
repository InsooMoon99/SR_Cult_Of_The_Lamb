#pragma once
#include "CNPCState.h"

/// NPC에게 말거는 상태
class CNPCState_Talk : public CNPCState
{
private:
    explicit CNPCState_Talk(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Talk() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Talk* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    _float m_Timer; // 자동 상태 넘기기 타이머
};
