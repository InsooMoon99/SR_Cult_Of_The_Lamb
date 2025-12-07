#include "pch.h"
#include "CUICharging.h"
#include "CCreateHelper.h"
#include "CPlayerSkill.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"

CUICharging::CUICharging(DEVICE graphicDev): CUI(graphicDev) {}
CUICharging::CUICharging(const CUICharging& rhs): CUI(rhs) {}
CUICharging::~CUICharging() {}

void CUICharging::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
}

HRESULT CUICharging::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Scale(50.f,50.f,0.f);
    _vec3 spawnOffset = m_PlayerSkill->Get_Owner()->Get_Transform()->Get_Pos();
    m_TransformCom->Set_Pos(spawnOffset);


    return S_OK;
}

void CUICharging::Render_GameObject()
{
    CUI::Render_GameObject();
	m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
	
    m_TextureCom->Set_Texture(10);
    m_BufferCom->Render_Buffer();
}

int CUICharging::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    float chargeTime = m_PlayerSkill->Get_ChargeTime();
    float maxChargeTime = m_PlayerSkill->Get_MaxChargeTime();
    m_ChargeRatio = chargeTime / maxChargeTime;
	//m_TransformCom->Set_Scale(m_ChargeRatio*, 0.f, 0.f);
    return exit;
}

HRESULT CUICharging::Add_Component()
{
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_CIRCLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_ICON, m_TextureCom });

    return S_OK;
}

CUICharging* CUICharging::Create(DEVICE graphicDev)
{
    auto chargingUI = new CUICharging(graphicDev);

    if(FAILED(chargingUI->Ready_GameObject()))
    {
        Safe_Release(chargingUI);
        MSG_BOX("ChargingUI Create Failed");
        return nullptr;
    }
}

void CUICharging::Free()
{
    CUI::Free();
}
