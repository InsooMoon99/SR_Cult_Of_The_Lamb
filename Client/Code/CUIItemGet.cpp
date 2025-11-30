#include "pch.h"
#include "CUIItemGet.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRcTex.h"

CUIItemGet::CUIItemGet(DEVICE graphicDev) : CUI(graphicDev)
{
    
}

CUIItemGet::CUIItemGet(const CUIItemGet& rhs) : CUI(rhs)
{
    
}
CUIItemGet::~CUIItemGet()
{
    
}

HRESULT CUIItemGet::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;



    return S_OK;
}

_int CUIItemGet::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    

    return exit;
}

void CUIItemGet::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIItemGet::Render_GameObject()
{
    CUI::Render_GameObject();

    // 배경 이미지 렌더
    m_TextureCom->Set_Texture(8);
    m_BufferCom->Render_Buffer();
}

HRESULT CUIItemGet::Add_Component()
{
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({COMPONENTTYPE::RC_TEX, m_BufferCom});

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_ICON, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM );
    NULL_CHECK_RETURN(m_TransformCom);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });



    return S_OK;



}

void CUIItemGet::Add_UIItem(ItemInstance* iteminstance)
{
     m_UIItem.itemId = iteminstance->itemId;
     m_UIItem.remainTime = 2.f;
     m_UIItem.count++;
     m_UIItems.push_back(m_UIItem);
}

CUIItemGet* CUIItemGet::Create(DEVICE graphicDev)
{

    auto itemGet = new CUIItemGet(graphicDev);
    if(FAILED(itemGet->Ready_GameObject()))
    {
        Safe_Release(itemGet);
        MSG_BOX("CUI_ItemGet Created Failed");
        return nullptr;
    }

    return itemGet;
}

void CUIItemGet::Render_Editor()
{
    CUI::Render_Editor();
}

void CUIItemGet::Free()
{
    CUI::Free();
}
