#pragma once

#include "CStateMachine.h"

class CNPCBase;
class CNPCStateMachine : public CStateMachine
{
public:
    enum class NPC_STATE
    {
        IDLE,
        RUN,
        WALKPAST,       // 이동 중 플레이어와 가까워졌을 때
        BUILD,          // 건축
        CHOPWOOD,       // 벌목
        MINING,         // 채광
        EAT,            // 식사
        PRAY,           // 기도
        PRAY_PEEK,      // 기도 중 딴짓
        SPAWN,          // 소환
        
        STATE_END
    };

protected:
    explicit CNPCStateMachine();
    ~CNPCStateMachine() override;

public:
    HRESULT Ready_StateMachine() override;
    _int    Update_StateMachine(const _float& timeDelta) override;
    void    Change_State(_uint dwStateKey) override;

public:
    static CNPCStateMachine* Create(CNPCBase* owner);

protected:
    void Free() override;

private:
    CNPCBase* m_Owner = nullptr;

public:
    void Set_Owner(CNPCBase* owner) { m_Owner = owner; }

    template<typename TState>
    HRESULT RegisterState(NPC_STATE stateEnum, CNPCBase* owner)
    {
        CBaseState* state = TState::Create(owner, this);
        if (!state)
            return E_FAIL;
        
        m_vecState[static_cast<_uint>(stateEnum)] = state;
        return S_OK;
    }
};
