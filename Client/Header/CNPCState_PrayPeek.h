#pragma once
#include "CNPCState.h"

/// @brief NPC의 기도 중 딴짓(PrayPeek) 상태
class CNPCState_PrayPeek : public CNPCState
{
private:
    explicit CNPCState_PrayPeek(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_PrayPeek() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_PrayPeek* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
