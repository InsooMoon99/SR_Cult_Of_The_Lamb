#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CMonsterBase;

class CEyeball_Attack : public CMonsterState
{
private:
    explicit CEyeball_Attack(CMonsterBase* owner);
    virtual ~CEyeball_Attack();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Attack* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;

    _vec3               m_StartPos{};         
    _vec3               m_TargetPos{};        
    _float              m_LerpSpeed;        
    _bool               m_IsReachedTarget;  


};

