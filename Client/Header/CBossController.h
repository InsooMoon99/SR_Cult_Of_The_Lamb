#pragma once

#include "CComponent.h"

class CStateMachine;

BEGIN(Engine)
class CAnimator;
class CCombatStat;
END

class CBossController : public CComponent
{
private:
    explicit CBossController(DEVICE graphicDev);
    explicit CBossController(const CBossController& rhs);
    virtual ~CBossController();

public:
    HRESULT Ready_BossController();
    void Initialize_Component() override;
    _int Update_Component(const _float& timeDelta) override;

    void Set_StateMachine(CStateMachine* stateMachine);
    CStateMachine* Get_StateMachine() { return m_StateMachine; }

    void Change_State(_int stateKey);
    _int Get_CurrentState();

public:
    CTransform*     Get_OwnerTransform();
    CAnimator*      Get_OwnerAnimator();
    CCombatStat*    Get_OwnerCombatStat();

public:
    static CBossController* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Cache_Components();
    void Free() override;

private:
    CAnimator*      m_AnimatorCom;
    CCombatStat*    m_CombatStatCom;
    CStateMachine*  m_StateMachine;

    _bool           m_ComponentsCached;


};

