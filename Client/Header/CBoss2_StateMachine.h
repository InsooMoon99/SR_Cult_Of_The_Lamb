#pragma once

#include "CStateMachine.h"

class CMonsterBase;
class CMonsterState;

BEGIN(Engine)
class CGameObject;
END

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

private:
    CMonsterBase* m_Owner;

    template<typename TState>
    HRESULT RegisterState(_uint stateIndex, CMonsterBase* owner)
    {
        CMonsterState* state = TState::Create(owner);
        if (!state)
        {
            MSG_BOX("State Create Faield");
            return E_FAIL;
        }

        //!! 석호 : 제쪽 에러 발생으로 잠시 비활성화했습니다
        //m_vecState[stateIndex] = state;

        return S_OK;
    }
};

