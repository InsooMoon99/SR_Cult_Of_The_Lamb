#include "pch.h"
#include "CUIItemGet.h"

#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
class CItemDB;

CUIItemGet::CUIItemGet(DEVICE graphicDev)
    : CUI(graphicDev),
      m_UIItemIcon(nullptr),
      m_UIItem{},
      m_IsOpen(false),
      m_IsAnimating(false),
      m_CloseAnimation(false),
      m_OpenAnimaiton(false),
      m_TargetX(-550.f),
      m_ClosedX(-1000.f),
      m_Speed(100.f)
{ }

CUIItemGet::CUIItemGet(const CUIItemGet& rhs)
    : CUI(rhs),
      m_UIItemIcon(nullptr),
      m_UIItem{},
      m_IsOpen(false),
      m_IsAnimating(false),
      m_CloseAnimation(false),
      m_OpenAnimaiton(false),
      m_TargetX(-550.f),
      m_ClosedX(-600.f),
      m_Speed(250.f)
{ }

CUIItemGet::~CUIItemGet()
{ }

HRESULT CUIItemGet::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    //아이템 텍스처
    m_UIItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    m_UIItemIcon->SetRegister(false);
    m_UIItemIcon->Get_Transform()->Set_Scale(25.f, 25.f, 0.f);

    //배경 위치
    m_TransformCom->Set_Scale(165.f, 36.9f, 0.f);
    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 0.f);
    return S_OK;
}

_int CUIItemGet::Update_GameObject(const _float& timeDelta)
{
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    int exit = CUI::Update_GameObject(timeDelta);

    int  index = CItemDB::GetInstance()->GetIndexById(m_UIItem.itemId);
    m_UIItemIcon->Set_TextureIndex(index);
    m_UIItemIcon->Update_GameObject(timeDelta);

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_5))
        m_IsAnimating = true;

    if (m_IsAnimating)
    {
        float targetX = m_IsOpen ? m_TargetX : m_ClosedX; // 열림 vs 닫힘 상태에 따라서 목표 X좌표 결정

        _vec3 pos = m_TransformCom->Get_Pos();

        int dir = targetX < pos.x ? -1 : 1; // 이동 방향 설정

        float uiItemOffset = 50.f;

        pos.x += timeDelta * m_Speed * dir;

        m_TransformCom->Set_Pos(pos);
        m_UIItemIcon->Get_Transform()->Set_Pos(pos.x + 50.f, pos.y, pos.z);

        if (dir <= 0 && (pos.x <= m_ClosedX) || (dir >= 0 && (m_TargetX <= pos.x)))

        {
            // m_IsAnimating = false;
            m_IsOpen = !m_IsOpen;
        }
    }
    return exit;
}

void CUIItemGet::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIItemGet::Render_GameObject()
{
    CUI::Render_GameObject();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if (m_IsAnimating)
    {
        // 배경 이미지 렌더
        m_TextureCom->Set_Texture(8);
        m_BufferCom->Render_Buffer();

        // 임시 아이콘 렌더
        m_UIItemIcon->Render_GameObject();

        cout << m_TransformCom->Get_Pos().x << "\t" << m_TransformCom->Get_Pos().y << endl;
    }
}

HRESULT CUIItemGet::Add_Component()
{
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_ICON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_ICON, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    return S_OK;
}

//
//void CUIItemGet::Render_Setting()
//{
//  
//   m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//
//   m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//   m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//   m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//
//   m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
//   m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
//   m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
//
//}
//
void CUIItemGet::Set_UIItem(int itemId, int itemCount)
{
     m_UIItem.itemId = itemId;
     m_UIItem.count += itemCount;
     m_UIItem.remainTime = 2.f;

}

CUIItemGet* CUIItemGet::Create(DEVICE graphicDev)
{
    auto itemGet = new CUIItemGet(graphicDev);
    if (FAILED(itemGet->Ready_GameObject()))
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
    Safe_Release(m_UIItemIcon);

    CUI::Free();
}
