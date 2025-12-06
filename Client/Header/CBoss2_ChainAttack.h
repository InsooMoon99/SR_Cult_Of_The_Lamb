#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
class CRcTex;
END

class CBoss2_ChainAttack : public CMonsterState
{
public:
    enum ChainAttackAnim
    {
        ROAR,       // 시작
        HANDSLMA,   // 공격

        ChainAnim_END
    };

private:
    explicit CBoss2_ChainAttack(CGameObject* owner);
    virtual ~CBoss2_ChainAttack();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    void PlayRoar();
    void PlayHandSlam();

    void SpawnChain();

public:
    static CBoss2_ChainAttack* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*     m_Owner;

    _float           m_Pattern_Cooldown;
    ChainAttackAnim  m_ChainAnim = ChainAttackAnim::ChainAnim_END;

    _int             m_SlamCount = 0;
    _bool            m_IsFinished = false;

private:
    _float           m_ChainSpawnTimer = 0.f;
    _int             m_ChainCount = 0;
    _int             m_MaxChainCount = 20;
    _vec3            m_ChainStartPos  {};       
    _vec3            m_ChainDirection {};      
    _float           m_ChainSpacing = 1.5f; 
    _bool            m_HasSpawnedWarning = false;

};

