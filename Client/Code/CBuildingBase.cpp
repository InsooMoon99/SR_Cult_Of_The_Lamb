#include "pch.h"
#include "CBuildingBase.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTransform.h"

CBuildingBase::CBuildingBase(DEVICE graphicDev)
    : CRenderObject(graphicDev)
{ }

CBuildingBase::CBuildingBase(const CBuildingBase& rhs)
    : CRenderObject(rhs)
{ }

CBuildingBase::~CBuildingBase()
{ }

HRESULT CBuildingBase::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CBuildingBase::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CBuildingBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    if (!m_IsPreview)
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return exit;
}

void CBuildingBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CBuildingBase::Render_GameObject()
{
    if (m_IsPreview)
    {
        if (m_TransformCom)
            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        if (m_BufferCom)
            m_BufferCom->Render_Buffer();
    }
    else
    {
        CRenderObject::Render_GameObject();
    }

    if (m_BoxCollCom)
        m_BoxCollCom->Render();
}

void CBuildingBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnBeginOverlap(self, other);
}

void CBuildingBase::OnStayOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnStayOverlap(self, other);
}

void CBuildingBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnEndOverlap(self, other);
}

HRESULT CBuildingBase::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // BoxCollider
    m_BoxCollCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxCollCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxCollCom });

    return S_OK;
}

void CBuildingBase::Free()
{
    CRenderObject::Free();
}
