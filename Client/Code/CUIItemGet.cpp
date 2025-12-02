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
#include "CFontMgr.h"

class CItemDB;

CUIItemGet::CUIItemGet(DEVICE graphicDev)
    : CUI(graphicDev),
    m_UIItemIcon(nullptr),
    m_UIItem{},
    m_IsOpen(false),
    m_IsAnimating(false),
    m_CloseAnimation(false),
    m_OpenAnimaiton(false),
    m_TargetX(-500.f),
    m_ClosedX(-900.f),
    m_Speed(250.f),
    m_IsDead(false)
{ }

CUIItemGet::CUIItemGet(const CUIItemGet& rhs)
    : CUI(rhs),
      m_UIItemIcon(nullptr),
      m_UIItem{},
      m_IsOpen(false),
      m_IsAnimating(false),
      m_CloseAnimation(false),
      m_OpenAnimaiton(false),
      m_TargetX(-500.f),
      m_ClosedX(-900.f),
      m_Speed(250.f),
      m_IsDead(false)
{ }

CUIItemGet::~CUIItemGet()
{
    
}

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
    m_TransformCom->Set_Pos(-950.f, 100.f, 0.f);
    return S_OK;
}

_int CUIItemGet::Update_GameObject(const _float& timeDelta)
{
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    int exit = CUI::Update_GameObject(timeDelta);

    if (m_UIItem.itemId)
    {
		m_UIItem.remainTime -= timeDelta;
        int  index = CItemDB::GetInstance()->GetIndexById(m_UIItem.itemId);
        m_UIItemIcon->Set_TextureIndex(index);
        m_UIItemIcon->Update_GameObject(timeDelta);
		
        float targetX = m_IsOpen ? m_TargetX : m_ClosedX; // 열림 vs 닫힘 상태에 따라서 목표 X좌표 결정

        _vec3 pos = m_TransformCom->Get_Pos();

        int dir = targetX < pos.x ? -1 : 1; // 이동 방향 설정

        float uiItemOffset = 50.f;

        pos.x += timeDelta * m_Speed * dir;

        m_TransformCom->Set_Pos(pos.x, (- m_Index) *  80.f + 60.f, pos.z);
        m_UIItemIcon->Get_Transform()->Set_Pos(pos.x -30.f, ( - m_Index) *  80.f+ 60.f, pos.z);

        if (dir <= 0 && (pos.x <= m_ClosedX) || (dir >= 0 && (m_TargetX <= pos.x)))
        {
            m_IsOpen = !m_IsOpen;
        }

		if(m_UIItem.remainTime <= 0)
		{
			m_IsDead = true;
		}
		//if (pos.x<=m_ClosedX)
		//
		//{
		//
		//	m_IsDead = true;
		//
		//}
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

    if (m_UIItem.itemId)
    {
        // ------------------------------------------------------------------
    // 1. [상태 저장]: 현재 GPU 상태를 백업 변수에 저장합니다.
    // ------------------------------------------------------------------
        DWORD dwOldAlphaEnable;
        DWORD dwOldSrcBlend;
        DWORD dwOldDestBlend;
        DWORD dwOldColorOp;
        DWORD dwOldColorArg1;
        DWORD dwOldTextureFactor; // 🛑 텍스처 팩터 색상도 저장합니다.

        m_GraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwOldAlphaEnable);
        m_GraphicDev->GetRenderState(D3DRS_SRCBLEND, &dwOldSrcBlend);
        m_GraphicDev->GetRenderState(D3DRS_DESTBLEND, &dwOldDestBlend);
        m_GraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor); // 팩터 색상 저장
        m_GraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
        m_GraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);

        // ------------------------------------------------------------------
        // 2. [상태 설정]: 원하는 투명도 및 색상 곱셈 상태로 변경합니다.
        // ------------------------------------------------------------------

        // 2-1. 투명도 조절 (예: 50% 투명, 알파 = 128)
        D3DCOLOR fadeColor = D3DCOLOR_ARGB(128, 255, 255, 255);
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, fadeColor);

        // 2-2. 알파 블렌딩 활성화 및 표준 공식 설정
        m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        // 2-3. 색상 곱셈 설정 (텍스처 색상 * 전역 색상/알파)
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); // TFACTOR(fadeColor)와 곱함
        // 배경 이미지 렌더
        m_TextureCom->Set_Texture(8);
        m_BufferCom->Render_Buffer();

        // ------------------------------------------------------------------
    // 4. [상태 복원]: 저장해 둔 원래의 상태로 되돌립니다.
    // ------------------------------------------------------------------
        m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwOldAlphaEnable);
        m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, dwOldSrcBlend);
        m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, dwOldDestBlend);
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor); // 원래 TEXTUREFACTOR로 복원
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
    }
        _vec3 pos = m_TransformCom->Get_Pos();
        // 아이콘 렌더
        m_UIItemIcon->Render_GameObject();

        Item* item = CItemDB::GetInstance()->GetItemById(m_UIItem.itemId);
		_vec2 screen = { WINCX / 2 + pos.x +50.f, WINCY / 2 - pos.y};
		CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Small", CItemDB::GetInstance()->Utf8ToWstring(item->name) + L" +" + to_wstring(m_UIItem.count), &screen, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

    //   screen = { WINCX / 2 + pos.x + 50.f, WINCY / 2 - pos.y };
    //CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Small", to_wstring(m_UIItem.count), &screen, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));
    //    //cout << m_TransformCom->Get_Pos().x << "\t" << m_TransformCom->Get_Pos().y << endl;
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

void CUIItemGet::Set_UIItem(int itemId, int itemCount)
{
    if (!m_IsDead && m_UIItem.itemId == itemId)
    {
        m_UIItem.itemId = itemId;
        int oldCount = m_UIItem.count;
        m_UIItem.count += itemCount;
        m_UIItem.newcount = m_UIItem.count - oldCount;
    }
    else
    {
        m_UIItem.itemId = itemId;
        m_UIItem.count = itemCount;
        m_UIItem.newcount = itemCount;
        m_UIItem.remainTime = 3.5f;
    }
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
