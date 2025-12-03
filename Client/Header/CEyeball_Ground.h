#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_Ground : public CMonsterState
{
private:
    explicit CEyeball_Ground(CMonsterBase* owner);
    virtual ~CEyeball_Ground();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Ground* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;

    _vec3               m_StartPos{};
    _vec3               m_TargetDir{};
    _vec3               m_TargetPos{};

    _float              m_LerpSpeed;
    _float              m_MaxMoveDistance;
};

