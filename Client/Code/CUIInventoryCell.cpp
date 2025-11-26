#include "pch.h"
#include "CUIInventoryCell.h"

#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

CUIInventoryCell::CUIInventoryCell(DEVICE pGraphicDev)
    : CUI(pGraphicDev),
      m_ItemTexCom(nullptr),
      m_HighlightTexCom(nullptr),
      m_index(0),
      m_Count(0),
      m_IsHovered(false)
{ }

CUIInventoryCell::CUIInventoryCell(const CUIInventoryCell& rhs)
    : CUI(rhs),
      m_ItemTexCom(nullptr),
      m_HighlightTexCom(nullptr),
      m_index(0),
      m_Count(0),
      m_IsHovered(false)
{ }

CUIInventoryCell::~CUIInventoryCell()
{ }

void CUIInventoryCell::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUIInventoryCell::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

void CUIInventoryCell::Render_Editor()
{
    CUI::Render_Editor();
}

HRESULT CUIInventoryCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //m_TransformCom->Set_Pos(_vec3(-550.f, 270.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(30.f, 30.f, 0.f));

    return S_OK;
}

_int CUIInventoryCell::Update_GameObject(const _float& timeDelta)
{
    // 외곽선 잘 바뀌는지 test
    if (m_IsHovered)
        m_IsHovered = false;

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_4) & 0x80)
        m_IsHovered = true;
    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIInventoryCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIInventoryCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if (!(m_ItemInst.itemId == -1))

    {
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(180, 100, 100, 100));

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // 무시됨
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

        m_TextureCom->Set_Texture(0);
        m_BufferCom->Render_Buffer();

        // 되돌리기
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

        m_index = CItemDB::GetInstance()->GetIndexById(m_ItemInst.itemId);

        m_ItemTexCom->Set_Texture(m_index);
        m_BufferCom->Render_Buffer();

        //cout << m_ItemInst.itemId << endl;
    }

    //TODO 유림 : 외곽선 그리기 인벤토리에서 셀 위치 확인해서 m_IsHovered 세팅하기
    //외곽선 테스트용
    if (m_IsHovered)
    {
        m_HighlightTexCom->Set_Texture(1);

        //외곽선 빨강으로 바꾸기
        D3DCOLOR color = D3DCOLOR_ARGB(255, 200, 0, 0);

        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //색상을 곱하기 방식으로 처리하곘다는 뜻
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //첫번째 인자는 텍스처
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); //두번째 인자는 색상
        m_BufferCom->Render_Buffer();

        //RenderState 원상태로 돌리기
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255, 255, 255, 255));

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    }
}

HRESULT CUIInventoryCell::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // cell texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_CELL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_CELL, m_TextureCom });

    // item texture
    m_ItemTexCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_ITEM);
    NULL_CHECK_RETURN(m_ItemTexCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_ITEM, m_ItemTexCom });

    //highlight texture
    m_HighlightTexCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_HIGHLIGHT);
    NULL_CHECK_RETURN(m_HighlightTexCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_HIGHLIGHT, m_HighlightTexCom });

    return S_OK;
}

CUIInventoryCell* CUIInventoryCell::Create(DEVICE graphicDev)
{
    auto cell = new CUIInventoryCell(graphicDev);
    if (FAILED(cell->Ready_GameObject()))
    {
        Safe_Release(cell);
        MSG_BOX("InventoryCell Created Failed");
        return nullptr;
    }
    return cell;
}

void CUIInventoryCell::Free()
{
    CUI::Free();
}
