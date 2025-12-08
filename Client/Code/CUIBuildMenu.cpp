#include "pch.h"
#include "CUIBuildMenu.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CUIBuildCell.h"
#include "CDInputMgr.h"
#include "CRenderer.h"

CUIBuildMenu::CUIBuildMenu(DEVICE graphicDev)
    : CUI(graphicDev),
      m_CookingFireCell(nullptr),
      m_TempleCell(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false) { }

CUIBuildMenu::CUIBuildMenu(const CUI& rhs) : CUI(rhs) { }

CUIBuildMenu::~CUIBuildMenu()
{
    
}
HRESULT CUIBuildMenu::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TargetX = 0;
    m_ClosedX = -WINCX;
    m_Speed = 900.f;

    //건축물 아이템 셀
    m_CookingFireCell = CUIBuildCell::Create(m_GraphicDev);
    m_CookingFireCell->Set_BuildIndex(BUILDITEM::COOKINGFIRE);
    m_CookingFireCell->Set_TextureIndex(0);
    m_BuildCells.push_back(m_CookingFireCell);

    m_TempleCell = CUIBuildCell::Create(m_GraphicDev);
    m_TempleCell->Set_BuildIndex(BUILDITEM::TEMPLE);
    m_TempleCell->Set_TextureIndex(1);
    m_BuildCells.push_back(m_TempleCell);

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);
    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 0.f);
    return S_OK;
}

_int CUIBuildMenu::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
    {
        m_IsOpen = !m_IsOpen;
        m_IsAnimating = true;
    }

    float targetX = m_IsOpen ? m_TargetX : m_ClosedX; //열려있냐 닫혀있냐에 따라서 목표 X값이 달라짐
    _vec3 pos = m_TransformCom->Get_Pos();

    if (m_IsAnimating)
    {
        float dir = targetX < pos.x ? -1.f : 1.f; //현재 x좌표에 따라서 움직일 방향이 달라짐
        pos.x += dir * m_Speed * timeDelta;

        if ((dir > 0.f && pos.x >= targetX) || (dir < 0.f && pos.x <= targetX)) //목표한 X에 도달하면 애니메이션 동작 꺼주기
        {
            pos.x = targetX;
            m_IsAnimating = false;
        }
        m_TransformCom->Set_Pos(pos);
    }

    int cellsSize = m_BuildCells.size();
    for (int i = 0; i < cellsSize; i++)
    {
        m_BuildCells[i]->Render_GameObject();
    }
    Check_Hovered();

    return exit;
}

void CUIBuildMenu::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //배경 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();
;
    int cellsSize = m_BuildCells.size();
    for (int i = 0; i < cellsSize; i++)
    {
        m_BuildCells[i]->Render_GameObject();
    }
}

void CUIBuildMenu::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

HRESULT       CUIBuildMenu::Add_Component()
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

CUIBuildMenu* CUIBuildMenu::Create(DEVICE graphicDev)
{
    auto buildMenu = new CUIBuildMenu(graphicDev);

    if (FAILED(buildMenu->Ready_GameObject()))
    {
        Safe_Release(buildMenu);
        MSG_BOX("UI_BuildMenu Create Failed");
        return nullptr;
    }
    return buildMenu;
}

void CUIBuildMenu::Free()
{
    CUI::Free();
}

void CUIBuildMenu::Initialize()
{
    
}

void CUIBuildMenu::Check_Hovered()
{
    for (int i = 0; i < m_BuildCells.size(); i++)
    {
        _vec3 cellPos = dynamic_cast<CTransform*>(m_BuildCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();
        _vec3 cellSize = dynamic_cast<CTransform*>(m_BuildCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Scale();

        float screenX = cellPos.x + WINCX * 0.5f;
        float screenY = WINCY * 0.5f - cellPos.y;

        m_tRect.left = (LONG)(screenX - cellSize.x);
        m_tRect.top = (LONG)(screenY - cellSize.y);
        m_tRect.right = (LONG)(screenX + cellSize.x);
        m_tRect.bottom = (LONG)(screenY + cellSize.y);

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);
        bool hoveredNow = PtInRect(&m_tRect, pt) != 0;

        if (hoveredNow != m_BuildCells[i]->Get_IsHovered())
        {
            m_BuildCells[i]->Set_IsHovered(hoveredNow);
        }

    }
}
