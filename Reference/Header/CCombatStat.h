#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CAnimator;

typedef struct tagHitData
{
    _vec3        dir;        // 넉백 방향
    _float       damage;     // 데미지
    _float       knockback;  // 넉백량
    _float       duration;   // 넉백 유지 시간(필요하면 사용 ㄱㄱ)

    CGameObject* attacker;   // 때린 놈
    CGameObject* hitter;     // 맞은 놈

    tagHitData()
        : dir(0.f, 0.f, 0.f)
        , damage(0.f)
        , knockback(0.f)
        , duration(0.f)
        , attacker(nullptr)
        , hitter(nullptr)
    {}
} HITDATA;

/**
 * @brief GameObject의 전투 상태 스테이터스 컴포넌트
 */
class ENGINE_DLL CCombatStat : public CComponent
{
private:
    explicit CCombatStat(DEVICE graphicDev);
    explicit CCombatStat(const CCombatStat& rhs);
    ~CCombatStat() override;

public:
    HRESULT Ready_StateComponent(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    _float Get_Attack() { return m_Attack; }
    _float Get_Hp() { return m_Hp; }
    _float Get_MaxHp() { return m_MaxHp; }
    _float Get_Mp() { return m_Mp; }
    _float Get_Speed() { return m_Speed; }
    _float Get_MaxMp() { return m_MaxMp; }

    _bool  Is_Dead() { return m_IsDead; }

    void Set_Attack(_float attack) { m_Attack = attack; }
    void Set_Hp(_float hp) { m_Hp = hp; }
    void Set_MaxHp(_float maxHp) { m_MaxHp = maxHp; }
    void Set_Mp(_float mp) { m_Mp = mp; }
    void Set_MaxMp(_float maxMp) { m_MaxMp = maxMp; }
    void Set_Speed(_float speed) { m_Speed = speed; }

    void Add_Attack(float attack) { m_Attack += attack; }

public:
    void Take_Damage(HITDATA hitData);

    void Apply_Damage(const HITDATA& hitData);
    void Apply_Knockback(const HITDATA& hitData);
    void On_Dead();

public:
    static CCombatStat* Create(DEVICE graphicDev);

    CComponent* Clone() override;

protected:
    void Free() override;

private:
    _float m_Attack;

    _float m_Hp;
    _float m_MaxHp = 6.f;
    _float m_Mp;
    _float m_MaxMp = 10.f;
    _float m_Speed;

    _bool m_IsDead;
};

END
