#include "pch.h"
#include "CUIWeaponExplain.h"

#include "CCreateHelper.h"
#include "CFontMgr.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"

CUIWeaponExplain::CUIWeaponExplain(DEVICE graphicDev)
    : CUI(graphicDev),
      m_ItemID(0)
{ }

CUIWeaponExplain::CUIWeaponExplain(const CUIWeaponExplain& rhs)
    : CUI(rhs),
      m_ItemID(0)
{ }

CUIWeaponExplain::~CUIWeaponExplain()
{ }

HRESULT CUIWeaponExplain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CUIWeaponExplain::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    Set_IsActive(false);

    m_TransformCom->Set_Pos(0.f, -200.f, 0.f);
    m_TransformCom->Set_Scale(300.f, 130.f, 0.f);
}

_int CUIWeaponExplain::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
    return exit;
}

void CUIWeaponExplain::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIWeaponExplain::Render_GameObject()
{
    CUI::Render_GameObject();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //배경 그리기
    if (m_Active)
    {
        m_TextureCom->Set_Texture(11);
        m_BufferCom->Render_Buffer();

        //아이템 이름 출력
        Item* item = CItemDB::GetInstance()->GetItemById(m_ItemID);

        _vec3 pos        = m_TransformCom->Get_Pos();
        _vec2 descOffset = { 0.f, -60.f };
        _vec2 screenPos  = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
        CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
                                             CItemDB::GetInstance()->Utf8ToWstring(item->name),
                                             &screenPos,
                                             D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

        //아이템 메인 설명 출력
        descOffset = { 0.f, 65.f };
        screenPos  = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
        CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                                             CItemDB::GetInstance()->Utf8ToWstring(item->desc),
                                             &screenPos,
                                             D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

        //아이템 부연 설명 출력
        descOffset = { 0.f, -20.f };
        screenPos  = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
        CFontMgr::GetInstance()->Render_Font(L"부연 설명",
                                             CItemDB::GetInstance()->Utf8ToWstring(item->additionalDesc),
                                             &screenPos,
                                             D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 1.f });
    }
    //Item* WeaponGauntlet = CItemDB::GetInstance()->GetItemById(3002);
    //
    //_vec3 Gauntletpos = m_CardIcon->Get_Transform()->Get_Pos();
    //_vec2 GauntletDescOffset = { 0.f, 0.f };
    //_vec2 GaultetScreenPos = { WINCX / 2 + Gauntletpos.x, WINCY / 2 - Gauntletpos.y + GauntletDescOffset.y };
    //CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
    //    CItemDB::GetInstance()->Utf8ToWstring(WeaponGauntlet->name),
    //    &GaultetScreenPos,
    //    D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });
    //
    ////아이템 메인 설명 출력
    //GauntletDescOffset = { 0.f, 95.f };
    //GaultetScreenPos = { WINCX / 2 + Gauntletpos.x, WINCY / 2 - Gauntletpos.y + GauntletDescOffset.y };
    //CFontMgr::GetInstance()->Render_Font(L"카드 설명",
    //    CItemDB::GetInstance()->Utf8ToWstring(WeaponGauntlet->desc),
    //    &GaultetScreenPos,
    //    D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });
    //
    ////아이템 부연 설명 출력
    //GauntletDescOffset = { 0.f, 25.f };
    //GaultetScreenPos = { WINCX / 2 + GaultetScreenPos.x, WINCY / 2 - GaultetScreenPos.y + GauntletDescOffset.y };
    //CFontMgr::GetInstance()->Render_Font(L"부연 설명",
    //    CItemDB::GetInstance()->Utf8ToWstring(WeaponGauntlet->additionalDesc),
    //    &GaultetScreenPos,
    //    D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 1.f });
    //
}

HRESULT CUIWeaponExplain::Add_Component()
{
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_ICON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_ICON, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });
    return S_OK;
}

CUIWeaponExplain* CUIWeaponExplain::Create(DEVICE graphicDev)
{
    auto weaponExplainUI = new CUIWeaponExplain(graphicDev);
    if (FAILED(weaponExplainUI->Ready_GameObject()))
    {
        Safe_Release(weaponExplainUI);
        MSG_BOX("WeaponExplainUI Create Failed");
        return nullptr;
    }
    return weaponExplainUI;
}

void CUIWeaponExplain::Free()
{
    CUI::Free();
}
