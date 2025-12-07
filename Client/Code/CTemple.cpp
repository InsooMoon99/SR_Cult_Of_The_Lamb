#include "pch.h"
#include "CTemple.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"

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
    //TODO 출력 테스트 필요: 오프셋에 따라 두 텍스처가 다른 위치에 출력되고 있는지 검증해야 한다

    NULL_CHECK(m_TextureCom);

    // 앞면 텍스처 렌더링
    m_TextureCom->Set_Texture(0);

    CBuildingBase::Render_GameObject();

    // 다른 위치에 지붕 텍스처 렌더링
    _matrix matWorld = m_TransformCom->Get_World();
    _matrix matOffset;
    D3DXMatrixTranslation(&matOffset, 0.f, 0.f, 5.f);

    matWorld = matOffset * matWorld;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    m_TextureCom->Set_Texture(1);

    if (!m_IsPreview)
        Render_Setting();

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();
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

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TEMPLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TEMPLE, m_TextureCom });

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
