#include "CAttackBox.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTransform.h"

CAttackBox::CAttackBox(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BoxColCom(nullptr), m_TransformCom(nullptr),
      m_Instigator(nullptr),
      m_LifeTime(5.55f) // 테스트용으로 좀 길게
{
}

CAttackBox::CAttackBox(const CAttackBox& rhs)
    : CGameObject(rhs),
      m_BoxColCom(nullptr), m_TransformCom(nullptr),
      m_HitDesc(rhs.m_HitDesc),
      m_Instigator(rhs.m_Instigator),
      m_LifeTime(rhs.m_LifeTime)
{
}

CAttackBox::~CAttackBox()
{ }

HRESULT CAttackBox::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CAttackBox::Update_GameObject(const _float& timeDelta)
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

void CAttackBox::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CAttackBox::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_BoxColCom->Render();
}

void CAttackBox::OnBeginOverlap(CCollider* self, CCollider* other)
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
    HITDATA hit = m_HitDesc;
    hit.dir = dir;
    hit.attacker = m_Instigator;

    combat->Take_Damage(hit);

    // 데미지를 줬으면, 삭제 요청
    Mark_As_PendingKill();
}

void CAttackBox::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);

    
}

HRESULT CAttackBox::Add_Component()
{
    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].emplace(COMPONENTTYPE::TRANSFORM, m_TransformCom);

    // BoxCol
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);

    // 크기 기본값 지정
    m_BoxColCom->Set_Size(_vec3(3.f, 3.f, 3.f));

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    return S_OK;
}

CAttackBox* CAttackBox::Create(DEVICE graphicDev)
{
    CAttackBox* box = new CAttackBox(graphicDev);

    if (FAILED(box->Ready_GameObject()))
    {
        Safe_Release(box);
        MSG_BOX("AttackBox Create Failed");
        return nullptr;
    }

    return box;
}

void CAttackBox::Free()
{
    CGameObject::Free();


}
