#include "CAttackBase.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTransform.h"

CAttackBase::CAttackBase(DEVICE graphicDev)
    : CGameObject(graphicDev),
    m_TransformCom(nullptr),
    m_Instigator(nullptr),
    m_LifeTime(0.4f)
{
}

CAttackBase::CAttackBase(const CAttackBase& rhs)
    : CGameObject(rhs),
    m_TransformCom(nullptr),
    m_AttackData(rhs.m_AttackData),
    m_Instigator(rhs.m_Instigator),
    m_LifeTime(rhs.m_LifeTime)
{
}
CAttackBase::~CAttackBase()
{ }

_int CAttackBase::Update_GameObject(const _float& timeDelta)
{
    CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_NONALPHA, this);

    // LifeTime이 끝나면, 펜딩킬 마크해서 삭제 요청
    m_LifeTime -= timeDelta;
    if (m_LifeTime <= 0.f)
    {
        Mark_As_PendingKill();

        return 0;
    }

    return 0;
}

void CAttackBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CAttackBase::Render_GameObject()
{
    CGameObject::Render_GameObject();

    if (m_TransformCom)
    {
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    }

    CCollider* collider = Get_Collider();
    if (collider)
    {
        collider->Render();
    }
}

void CAttackBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);

    if (!m_Instigator || !other)
        return;

    CGameObject* hitter = other->Get_Owner();
    if (!hitter || hitter == m_Instigator)
        return;

    // 맞은놈 CombatStat 찾기
    auto combat = dynamic_cast<CCombatStat*>(
        hitter->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    if (!combat)
        return;

    // 방향 보정
    _vec3 atkPos = dynamic_cast<CTransform*>(
        m_Instigator->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();

    _vec3 hitterPos = dynamic_cast<CTransform*>(
        hitter->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();

    _vec3 dir = hitterPos - atkPos;
    if (D3DXVec3LengthSq(&dir) > 0.0001f)
        D3DXVec3Normalize(&dir, &dir);

    // 파싱된 데이터값으로 세팅?
    HITDATA hit(m_AttackData); 
    hit.dir = dir;             
    hit.attacker = m_Instigator;
    hit.hitter = hitter;

    combat->Take_Damage(hit);

    // 데미지를 줬으면, 삭제 요청 -> AttackBuilder에서 세팅
    if (m_DestroyOnHit)
    {
        Mark_As_PendingKill();
    }
}

void CAttackBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);

    
}

void CAttackBase::Set_Instigator(CGameObject* instigator)
{
    m_Instigator = instigator;

    CCollider* collider = Get_Collider();
    if (collider)
    {
        collider->Set_Instigator(instigator);
    }
}

void CAttackBase::Free()
{
    CGameObject::Free();

}
