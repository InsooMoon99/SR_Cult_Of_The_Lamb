#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
class CRcTex;
END

class CBoss2_FollowerDie : public CMonsterState
{
public:
    enum ChainAttackAnim
    {
        ROAR,       // 시작
        HANDSLMA,   // 공격

        ChainAnim_END
    };

private:
    explicit CBoss2_FollowerDie(CGameObject* owner);
    virtual ~CBoss2_FollowerDie();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CBoss2_FollowerDie* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*     m_Owner;

    _float           m_Pattern_Cooldown;
    ChainAttackAnim  m_ChainAnim = ChainAttackAnim::ChainAnim_END;

    _bool            m_IsFinished = false;


};

