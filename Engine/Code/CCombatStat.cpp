#include "CCombatStat.h"
#include "CAnimator.h"

CCombatStat::CCombatStat(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_Attack(0),
      m_Hp(0),
      m_Mp(0),
      m_Speed(20)
{ }

CCombatStat::CCombatStat(const CCombatStat& rhs)
    : CComponent(rhs),
      m_Attack(0),
      m_Hp(0),
      m_Mp(0),
      m_Speed(rhs.m_Speed)
{ }

CCombatStat::~CCombatStat()
{ }

HRESULT CCombatStat::Ready_StateComponent(DEVICE graphicDev)
{
    // TODO : 객체마다 스탯 설정해주기
    // ex) Player -> m_CombatStat->Set_Hp(100.f);
    // ex) Player -> m_CombatStat->Set_Attack(10.f);

    return S_OK;
}

_int CCombatStat::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    return exit;
}

void CCombatStat::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

CCombatStat* CCombatStat::Create(DEVICE graphicDev)
{
    CCombatStat* state = new CCombatStat(graphicDev);

    if (FAILED(state->Ready_StateComponent(graphicDev)))
    {
        Safe_Release(state);
        MSG_BOX("CombatStat Create Failed");
        return nullptr;
    }

    return state;
}

CComponent* CCombatStat::Clone()
{
    return new CCombatStat(*this);
}

void CCombatStat::Free()
{
    CComponent::Free();
}
