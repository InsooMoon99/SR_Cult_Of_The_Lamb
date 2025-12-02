#pragma once

#include "CStateMachine.h"

class CMonsterBase;

class CBoss2_StateMachine : public CStateMachine
{
private:
    explicit CBoss2_StateMachine(CMonsterBase* owner);
    virtual ~CBoss2_StateMachine() override;

public:
    HRESULT Ready_StateMachine() override;
    _int    Update_StateMachine(const _float& timeDelta) override;
    void    Change_State(_uint dwStateKey) override;

public:
    static CBoss2_StateMachine* Create(CMonsterBase* owner);

protected:
    void Free() override;
};

