#include "pch.h"
#include "CUIWeapon.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CScene.h"
#include "Engine_Enum.h"
#include "CWeaponEquip.h"
#include "CManagement.h"

class CWeaponEquip;
int CUIWeapon::m_NextID = 0;

CUIWeapon::CUIWeapon(DEVICE pGraphicDev) : CUI(pGraphicDev)
{
    m_ID = ++m_NextID;
}

CUIWeapon::CUIWeapon(const CUIWeapon& rhs) : CUI(rhs)
{
    m_ID = ++m_NextID;
}

CUIWeapon::~CUIWeapon()
{
}

HRESULT CUIWeapon::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(-500.f, 180.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(25.f, 25.f, 0.f));
    return S_OK;
}

_int CUIWeapon::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    CScene* scene = CManagement::GetInstance()->Get_Scene();

    CWeaponEquip* weaponEquip = dynamic_cast<CWeaponEquip*>(scene->Get_Component(COMPONENTID::ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::WEAPON_EQUIP));

    if (weaponEquip->Get_WeaponEquipped())
    {
        m_WeaponID = weaponEquip->Get_CurrentWeaponID();
        m_Invisible = false;
    }
    return exit;
}

void CUIWeapon::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIWeapon::Render_GameObject()
{
    if (!m_Invisible)
    {
        CUI::Render_GameObject();

        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        switch(m_WeaponID)
        {
        case 3001:
            m_TextureCom->Set_Texture(0);
            break;

        case 3002:
            m_TextureCom->Set_Texture(1);
            break;
        }

        m_BufferCom->Render_Buffer();
    }
}

HRESULT CUIWeapon::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_WEAPON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_WEAPON, m_TextureCom });

    return S_OK;
}

CUIWeapon* CUIWeapon::Create(DEVICE graphicDev)
{
    auto circle = new CUIWeapon(graphicDev);
    if (FAILED(circle->Ready_GameObject()))
    {
        Safe_Release(circle);
        MSG_BOX("UIHeartBar Created Failed");
        return nullptr;
    }
    return circle;
}

void CUIWeapon::Free()
{
    Safe_Release(m_BufferCom);
    Safe_Release(m_TextureCom);
    Safe_Release(m_TransformCom);

    CUI::Free();
}
