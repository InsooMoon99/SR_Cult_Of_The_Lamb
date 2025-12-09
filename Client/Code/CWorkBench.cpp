#include "pch.h"
#include "CWorkBench.h"

#include "CBoxCollider.h"
#include "CTransform.h"

CWorkBench::CWorkBench(DEVICE graphicDev)
    : CBuildingBase(graphicDev)
{ }

CWorkBench::CWorkBench(const CWorkBench& rhs)
    : CBuildingBase(rhs)
{ }

CWorkBench::~CWorkBench()
{ }

HRESULT CWorkBench::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    if (m_BoxCollCom)
        m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::BUILDING);

    return S_OK;
}

void CWorkBench::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_Scale(2.f, 2.f, 1.f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
}

_int CWorkBench::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    return exit;
}

void CWorkBench::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CWorkBench::Render_GameObject()
{
    CBuildingBase::Render_GameObject();
}

void CWorkBench::Render_Editor()
{
    CBuildingBase::Render_Editor();
}

void CWorkBench::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);
}

void CWorkBench::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);
}

void CWorkBench::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);
}

HRESULT CWorkBench::Add_Component()
{
    return CBuildingBase::Add_Component();
}

CWorkBench* CWorkBench::Create(DEVICE graphicDev) { return nullptr; }

void CWorkBench::Free()
{
    CBuildingBase::Free();
}
