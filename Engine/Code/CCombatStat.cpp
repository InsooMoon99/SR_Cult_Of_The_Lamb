#include "CCombatStat.h"
#include "CAnimator.h"
#include "CGameObject.h"
#include "CState.h"
#include "CTransform.h"

CCombatStat::CCombatStat(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_Attack(0),
      m_Hp(0), m_MaxHp(6),
      m_Mp(0), m_MaxMp(10),
      m_Speed(15), m_IsDead(false)
{
}

CCombatStat::CCombatStat(const CCombatStat& rhs)
    : CComponent(rhs),
      m_Attack(0),
      m_Hp(0), m_MaxHp(rhs.m_MaxHp),
      m_Mp(0), m_MaxMp(rhs.m_MaxMp),
      m_Speed(rhs.m_Speed), m_IsDead(false)
{
    m_Hp = m_MaxHp;
    m_Mp = m_MaxMp;

    m_IsDead = false;
}

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

void CCombatStat::Take_Damage(HITDATA hitData)
{
    if (m_IsDead)
        return;

    if (hitData.damage <= 0.f)
        return;

    // 데미지
    Apply_Damage(hitData);

    // 넉백 : 추가구현하면 됨 지금 좀 이상함
    //Apply_Knockback(hitData);
}

void CCombatStat::Apply_Damage(const HITDATA& hitData)
{
    m_Hp -= hitData.damage;
    if (m_Hp < 0.f)
        m_Hp = 0.f;

    if (m_Hp <= 0.f && !m_IsDead)
        On_Dead();
}

void CCombatStat::Apply_Knockback(const HITDATA& hitData)
{
    if (hitData.knockback <= 0.f)
        return;

    if (m_Owner == nullptr)
        return;

    CTransform* transform = dynamic_cast<CTransform*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (!transform)
        return;

    _vec3 pos = transform->Get_Pos();

    _vec3 dir = hitData.dir;

    if (D3DXVec3LengthSq(&dir) < 1e-6f) //수치가 낮으면 무시
        return;

    D3DXVec3Normalize(&dir, &dir);

    pos += dir * hitData.knockback;

    transform->Set_Pos(pos);
}

void CCombatStat::On_Dead()
{
    m_IsDead = true;

    if (m_Owner)
    {
        CState* state = dynamic_cast<CState*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

        if (state)
            state->Change_State(ACTORSTATE::DEAD);

        // 여기서 Owner->Mark_As_PendingKill 호출하면 삭제됨

    }
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
