#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossChainAttack :
    public CMonsterState
{
public:
    enum class PHASE { PLAYERLINE, CROSS, RANDCHAIN, PHASE_END };
private:
    explicit CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossChainAttack();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);

    void PlayerLine(_vec3 PlayerDir, _int MaxCount, _float m_Offset, float Speed);
    bool PlayerLineUpdate(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;

public:
    static CBossChainAttack* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
private:
    PHASE m_ePhase;
    float m_fElapsedTime[(int)PHASE::PHASE_END];
    float m_fExtraElapsedTime[(int)PHASE::PHASE_END];

    int m_Count = 0;
    _vec3 m_PlayerDir;
    int LineCount = 0;
    bool m_LineFlag = false;
private:
    virtual void Free();

};

