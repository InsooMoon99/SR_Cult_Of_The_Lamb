#pragma once
#include "CBaseState.h"

BEGIN(Engine)
class CTransform;
class CAnimator;
END

class CNPCBase;
class CNPCController;
class CNPCStateMachine;
class CPlayer;

class CNPCState : public CBaseState
{
protected:
    explicit CNPCState(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

protected:
    void Find_Player();

protected:
    void Free() override;

protected:
    CNPCBase*         m_Owner;
    CNPCController*   m_Controller;
    CNPCStateMachine* m_StateMachine;

    Engine::CTransform* m_OwnerTransform = nullptr;
    Engine::CAnimator*  m_OwnerAnimator  = nullptr;

    CPlayer*            m_Player          = nullptr;
    Engine::CTransform* m_PlayerTransform = nullptr;

    _float m_CurStateElapsedTime = 0.f;
};
