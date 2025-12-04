#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CBoss2_Start : public CMonsterState
{
private:
    explicit CBoss2_Start(CMonsterBase* owner);
    virtual ~CBoss2_Start();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

private:
    void Spawn_Eyeball(_int index);

public:
    static CBoss2_Start* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*    m_Owner;
    _int            m_Eyeball_SpawnCount;

    _float           m_Pattern_Cooldown;

};

