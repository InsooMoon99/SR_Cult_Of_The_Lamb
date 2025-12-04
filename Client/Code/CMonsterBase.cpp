#include "pch.h"
#include "CMonsterBase.h"
#include "CCombatStat.h"
#include "CTransform.h"
#include "CBoxCollider.h"

CMonsterBase::CMonsterBase(DEVICE graphicDev)
    : CCharacterBase(graphicDev),
    m_Target(nullptr),
    m_TargetTransform(nullptr),
    m_StoppingDistance(2.f),
    m_AttackRange(2.5f),
    m_ChaseRange(10.f),
    m_AttackCooldown(0),
    m_AttackTimer(0),
    m_CanAttack(false)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
    : CCharacterBase(rhs),
    m_Target(nullptr),
    m_TargetTransform(nullptr),
    m_StoppingDistance(rhs.m_StoppingDistance),
    m_AttackRange(rhs.m_AttackRange),
    m_ChaseRange(rhs.m_ChaseRange),
    m_AttackCooldown(rhs.m_AttackCooldown),
    m_AttackTimer(rhs.m_AttackTimer),
    m_CanAttack(false)
{
}

CMonsterBase::~CMonsterBase()
{
}

HRESULT CMonsterBase::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Speed(3.f);

    if (m_BoxColCom)
    {
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ENEMY);
    }

    return S_OK;
}

void CMonsterBase::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();

   
}

_int CMonsterBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    if (m_Target) // 타겟의 방향 설정
    {
        _vec3 pos = m_TransformCom->Get_Pos();
        _vec3 targetPos = m_TargetTransform->Get_Pos();
        m_TargetDir = targetPos - pos;
    }

    return exit;
}

void CMonsterBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

_float CMonsterBase::Get_DistanceToTarget()
{
    if (!m_TargetTransform || !m_TransformCom)
        return 9999.f;

    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 monsterPos = m_TransformCom->Get_Pos();

    _vec3 diff = monsterPos - targetPos;

    return D3DXVec3Length(&diff);
}

_bool CMonsterBase::Is_InAttackRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_AttackRange;
}

_bool CMonsterBase::Is_InStoppingRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_StoppingDistance;
}

_bool CMonsterBase::Is_InChaseRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_ChaseRange;
}

_bool CMonsterBase::Is_OutofChaseRange() const
{
    return !Is_InChaseRange();
}

void CMonsterBase::Free()
{
    CCharacterBase::Free();
}
