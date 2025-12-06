#include "pch.h"
#include "CTemple.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CTexture.h"

CTemple::CTemple(DEVICE graphicDev)
    : CBuildingBase(graphicDev)
{ }

CTemple::CTemple(const CTemple& rhs)
    : CBuildingBase(rhs)
{ }

CTemple::~CTemple()
{ }

HRESULT CTemple::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::BUILDING);

    return S_OK;
}

void CTemple::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();
}

_int CTemple::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    return exit;
}

void CTemple::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CTemple::Render_GameObject()
{
    CBuildingBase::Render_GameObject();
}

void CTemple::Render_Editor()
{
    CBuildingBase::Render_Editor();

    ImGui::Text("=== Temple ===");
}

void CTemple::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);
}

void CTemple::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);
}

void CTemple::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);
}

HRESULT CTemple::Add_Component()
{
    if (FAILED(CBuildingBase::Add_Component()))
        return E_FAIL;

    // Texture : Temple_Front
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TEMPLE_FRONT);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TEMPLE_FRONT, m_TextureCom });

    // Texture : Temple_Roof
    m_TextureCom2 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TEMPLE_ROOF);
    NULL_CHECK_RETURN(m_TextureCom2, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TEMPLE_ROOF, m_TextureCom2 });

    return S_OK;
}

CTemple* CTemple::Create(DEVICE graphicDev)
{
    auto temple = new CTemple(graphicDev);

    if (FAILED(temple->Ready_GameObject()))
    {
        MSG_BOX("Temple Create Failed");
        Safe_Release(temple);
        return nullptr;
    }

    return temple;
}

void CTemple::Free()
{
    CBuildingBase::Free();
}
