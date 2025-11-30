#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;
class CBossBullet;


class CBossBulletAttack :
    public CMonsterState
{
public:
    enum class PHASE { CIRCLE, ROTCROSS_SPREAD, SPREAD ,PHASE_END};

private:
    explicit CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossBulletAttack();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

    bool CircleBullet(const _float& timeDelta, int num, int radius, float interval);
    bool CrossBullet(const _float& timeDelta, int num, int length, float interval);
    bool SpreadBullet(const _float& timeDelta, int num, float interval, float Speed, int angleOffset = 0);
   
private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossBulletAttack* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    PHASE m_ePhase;
    float m_fElapsedTime =0;
    float m_fExtraElapsedTime = 0;
    bool m_StartFlag = false;
    int m_Count = 0;

    vector<CBossBullet*> tmpBullet;
    bool m_TargetFlag = false;


private:
    virtual void Free();

};

