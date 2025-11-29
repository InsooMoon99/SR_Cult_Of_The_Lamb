#include "pch.h"
#include "CMonsterBase.h"
#include "CCombatStat.h"
#include "CTransform.h"
#include "CBoxCollider.h"

CMonsterBase::CMonsterBase(DEVICE graphicDev)
    : CCharacterBase(graphicDev), m_Target(nullptr), m_TargetTransform(nullptr)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
    : CCharacterBase(rhs), m_Target(nullptr), m_TargetTransform(nullptr)
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

void CMonsterBase::Free()
{
    CCharacterBase::Free();
}
