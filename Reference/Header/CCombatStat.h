#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CAnimator;

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

    void Set_Attack(_float attack) { m_Attack = attack; }
    void Set_Hp(_float hp) { m_Hp = hp; }
    void Set_MaxHp(_float maxHp) { m_MaxHp = maxHp; }
    void Set_Mp(_float mp) { m_Mp = mp; }
    void Set_MaxMp(_float maxMp) { m_MaxMp = maxMp; }
    void Set_Speed(_float speed) { m_Speed = speed; }

    void Add_Attack(float attack) { m_Attack += attack; }
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
};

END
