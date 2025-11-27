#include "pch.h"
#include "CUIPlayerPage.h"

#include <CCreateHelper.h>
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"

CUIPlayerPage::CUIPlayerPage(DEVICE pGraphicDev)
    : CUI(pGraphicDev)
{
}

CUIPlayerPage::CUIPlayerPage(const CUIPlayerPage& rhs)
    : CUI(rhs)
{
}

CUIPlayerPage::~CUIPlayerPage()
{
}

HRESULT CUIPlayerPage::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);

    return S_OK;
}

_int CUIPlayerPage::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIPlayerPage::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIPlayerPage::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //인벤토리 배경 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();
}

HRESULT CUIPlayerPage::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_INVEN);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_INVEN, m_TextureCom });

    return S_OK;
}


CUIPlayerPage* CUIPlayerPage::Create(DEVICE graphicDev)
{
    CUIPlayerPage* uiPlayerPage = new CUIPlayerPage(graphicDev);

    if (FAILED(uiPlayerPage->Ready_GameObject()))
    {
        Safe_Release(uiPlayerPage);
        MSG_BOX("uiPlayerPage Created Failed");
        return nullptr;
    }
    return uiPlayerPage;
}

void CUIPlayerPage::Free()
{
    CUI::Free();
}
