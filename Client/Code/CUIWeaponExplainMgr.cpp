#include "pch.h"
#include "CUIWeaponExplainMgr.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"

CUIWeaponExplainMgr::CUIWeaponExplainMgr(DEVICE graphicDev): CUI(graphicDev)
{
    
}

CUIWeaponExplainMgr::CUIWeaponExplainMgr(const CUIWeaponExplainMgr& rhs): CUI(rhs)
{
    
}

CUIWeaponExplainMgr::~CUIWeaponExplainMgr()
{
    
}

HRESULT CUIWeaponExplainMgr::Ready_GameObject()
{

    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}

void CUIWeaponExplainMgr::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
}

_int CUIWeaponExplainMgr::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

   // m_TransformCom->Set_Pos();

    return exit;
}

void CUIWeaponExplainMgr::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIWeaponExplainMgr::Render_GameObject()
{
    CUI::Render_GameObject();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(11);
    m_BufferCom->Render_Buffer();
}

HRESULT CUIWeaponExplainMgr::Add_Component()
{
    m_WeaponCardUI = CreateProtoComponent<CUIIcon>(this, COMPONENTTYPE::TEX_UI_ICON);
    NULL_CHECK_RETURN(m_WeaponCardUI, E_FAIL);
    m_Components[ID_STATIC].insert({COMPONENTTYPE::TEX_UI_ICON, m_WeaponCardUI });
    return S_OK;
}

void CUIWeaponExplainMgr::Free()
{
    CUI::Free();
}
