#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CMonsterBase;

class CEyeball_Attack : public CMonsterState
{
    enum class Eyeball_Anim
    {
        ATTACK_ANTI,
        ATTACK_LOOP,
        ATTACK_END,

        EYEBALL_ATTACK_END
    };

private:
    explicit CEyeball_Attack(CMonsterBase* owner);
    virtual ~CEyeball_Attack();

public:
    void    Enter() override;
    _int    Update_State(const _float& timeDelta) override;
    void    Exit() override;

    HRESULT Ready_State() override;

public:
    void Play_Anti();
    void Play_Loop();
    void Play_End();

public:
    void Spawn_BulletPattern();

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

    Eyeball_Anim        m_EyeballAnim;
    _bool               m_HasFiredBullet;

};

