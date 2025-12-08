#include "pch.h"
#include "CUIBuildCell.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CFontMgr.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "ItemData.h"

CUIBuildCell::CUIBuildCell(DEVICE pGraphicDev)
    : CUI(pGraphicDev),
    m_ItemIcon(nullptr),
    m_CardIcon(nullptr),
    m_CardItemIcon(nullptr),
    m_HighlightTexCom(nullptr),
    m_IsHovered(false)
{
}

CUIBuildCell::CUIBuildCell(const CUIBuildCell& rhs)
    : CUI(rhs),
    m_ItemIcon(nullptr),
    m_CardIcon(nullptr),
    m_CardItemIcon(nullptr),
    m_HighlightTexCom(nullptr),
    m_IsHovered(rhs.m_IsHovered)
{
}

CUIBuildCell::~CUIBuildCell()
{
}

void CUIBuildCell::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUIBuildCell::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

void CUIBuildCell::Render_Editor()
{
    CUI::Render_Editor();
}

HRESULT CUIBuildCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //인벤토리 아이템 그리기용 객체 생성
    m_TransformCom->Set_Scale(45.f, 45.f, 0.f);
    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_BUILD); //아이템 텍스처
    m_ItemIcon->SetRegister(false); // 아이템 텍스처 렌더 그룹에 중복 등록되는 것 막기 위한 플래그

    //카드 그리기용 객체 생성
    m_CardIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_RITUALCARD);
    m_CardIcon->SetRegister(false);
    m_CardIcon->Get_Transform()->Set_Scale(250.f, 180.f, 0.f);
    m_CardIcon->Get_Transform()->Set_Pos(370.f, 0.f, 0.f);

    //카드 아이템 그리기용 객체 생성
    m_CardItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_BUILD); //아이템 텍스처
    m_CardItemIcon->SetRegister(false);
    _vec3 cardPos = m_CardIcon->Get_Transform()->Get_Pos();
    _vec2 itemOffset = { 0.f, 70.f };
    m_CardItemIcon->Get_Transform()->Set_Pos(cardPos.x + itemOffset.x, cardPos.y + itemOffset.y, 0.f);
    return S_OK;
}

_int CUIBuildCell::Update_GameObject(const _float& timeDelta)
{

    if (m_IsHovered)
        m_IsHovered = false;

    if (m_ItemIcon)
        m_ItemIcon->Update_GameObject(timeDelta);

    if (m_CardIcon)
        m_CardIcon->Update_GameObject(timeDelta);

    if (m_CardItemIcon)
        m_CardItemIcon->Update_GameObject(timeDelta);

    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIBuildCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIBuildCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(180, 100, 100, 100));

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // 무시됨
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

 
    m_ItemIcon->Set_TextureIndex(m_TextureIndex);
    m_CardItemIcon->Set_TextureIndex(m_TextureIndex);
    auto iconTransform = m_ItemIcon->Get_Transform();
    iconTransform->Set_Pos(_vec3(
        m_TransformCom->Get_Pos().x,
        m_TransformCom->Get_Pos().y,
        m_TransformCom->Get_Pos().z));
    m_ItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));
    m_CardItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));
 
    m_ItemIcon->Render_GameObject();
 
    //셀 선택시 외곽선 그리기
    if (m_IsHovered)
    {
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_HighlightTexCom->Set_Texture(1);

        //외곽선 빨강으로 바꾸기
        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 0, 0);

        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //색상을 곱하기 방식으로 처리하곘다는 뜻
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //첫번째 인자는 텍스처
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); //두번째 인자는 색상
        m_BufferCom->Render_Buffer();

        //RenderState 원상태로 돌리기
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        
            //카드 그리기
            m_CardIcon->Render_GameObject();
            m_CardItemIcon->Render_GameObject();

            _vec3 pos = m_CardIcon->Get_Transform()->Get_Pos();
            _vec2 descOffset = { 0.f, 0.f };
            _vec2 screen = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
            _vec2 descOffset2 = { 0.f, 95.f };
            _vec2 screen2 = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset2.y };

        switch(m_BuildIndex)
        {
        case BUILDITEM::COOKINGFIRE:
            //아이템 이름 출력
            CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
              L"성지",
                &screen,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

            //아이템 메인 설명 출력
            CFontMgr::GetInstance()->Render_Font(L"카드 설명",
              L"추종자들이 기도를 드리는 곳. 성지에서 생성되는 헌신을 축적합니다.",
                &screen,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

           // //아이템 부연 설명 출력
           // descOffset = { 0.f, 25.f };
           // screen = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
           // CFontMgr::GetInstance()->Render_Font(L"부연 설명",
           //     ,
           //     &screen,
           //     D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 1.f });
        
            break;

        case BUILDITEM::TEMPLE:
            CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
                L"사원",
                &screen,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

   
            CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                L"설교와 의식을 수행하는 사원을 짓습니다.",
                &screen2,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });
            break;
        }
    }
}

HRESULT CUIBuildCell::Add_Component()
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

    //highlight texture
    m_HighlightTexCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_HIGHLIGHT);
    NULL_CHECK_RETURN(m_HighlightTexCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_HIGHLIGHT, m_HighlightTexCom });

    return S_OK;
}

CUIBuildCell* CUIBuildCell::Create(DEVICE graphicDev)
{
    auto cell = new CUIBuildCell(graphicDev);
    if (FAILED(cell->Ready_GameObject()))
    {
        Safe_Release(cell);
        MSG_BOX("InventoryCell Created Failed");
        return nullptr;
    }
    return cell;
}

void CUIBuildCell::Free()
{
    Safe_Release(m_ItemIcon);
    Safe_Release(m_HighlightTexCom);
    Safe_Release(m_CardIcon);
    Safe_Release(m_CardItemIcon);

    CUI::Free();
}
