#include "pch.h"
#include "CUIIcon.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

int CUIIcon::m_nextID = 0;

CUIIcon::CUIIcon(DEVICE pGraphicDev)
    : CUI(pGraphicDev)
{
    m_ID = ++m_nextID;
}

CUIIcon::CUIIcon(const CUIIcon& rhs)
    : CUI(rhs)
{
    m_ID = ++m_nextID;
}

CUIIcon::~CUIIcon()
{}

HRESULT CUIIcon::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(100.f, 100.f, 1.f));
    m_TransformCom->Set_Scale(_vec3(100.f, 100.f, 1.f));
    return S_OK;
}

_int CUIIcon::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUIIcon::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIIcon::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(m_textureIndex);

    m_BufferCom->Render_Buffer();
}

HRESULT CUIIcon::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UIICON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UIICON, m_TextureCom });

    return S_OK;
}

CUIIcon* CUIIcon::Create(DEVICE graphicDev)
{
    auto Icon = new CUIIcon(graphicDev);
    if (FAILED(Icon->Ready_GameObject()))
    {
        Safe_Release(Icon);
        MSG_BOX("Icon Created Failed");
        return nullptr;
    }
    return Icon;
}

void CUIIcon::Free()
{
    CUI::Free();
}
