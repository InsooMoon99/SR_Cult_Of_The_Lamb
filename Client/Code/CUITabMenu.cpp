#include "pch.h"
#include "CUITabMenu.h"

#include <CCreateHelper.h>
#include <CDInputMgr.h>
#include "CUIInventory.h"
#include "CUIButton.h"
#include "CTransform.h"
#include "CRenderer.h"

CUITabMenu::CUITabMenu(DEVICE graphicDev)
    : CUI(graphicDev),
      m_Current(UITabCategory::INVENTORY),
      m_InvenBtn(nullptr),
      m_PlayerBtn(nullptr),
      m_Inventory(nullptr),
      m_InventoryPage(nullptr),
      invenTransformCom(nullptr),
      invenBtnTransformCom(nullptr),
      playerBtnTransformCom(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false)
{}

CUITabMenu::CUITabMenu(const CUI& rhs)
    : CUI(rhs),
      m_Current(UITabCategory::INVENTORY),
      m_InvenBtn(nullptr),
      m_PlayerBtn(nullptr),
      m_Inventory(nullptr),
      m_InventoryPage(nullptr),
      invenTransformCom(nullptr),
      invenBtnTransformCom(nullptr),
      playerBtnTransformCom(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false)
{ }

CUITabMenu::~CUITabMenu()
{
    
}

CUITabMenu* CUITabMenu::Create(DEVICE graphicDev)
{
    auto tabMenu = new CUITabMenu(graphicDev);
    if (FAILED(tabMenu->Ready_GameObject()))
    {
        Safe_Release(tabMenu);
        MSG_BOX("CUITabMenu Created Failed");
        return nullptr;
    }
    return tabMenu;
}


HRESULT CUITabMenu::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TargetX = -WINCX / 5.f;
    m_ClosedX = -WINCX;
    m_Speed = 900.f;

    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 1.f);

    m_InventoryPage = CUIInventory::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_InventoryPage, E_FAIL);

    //TODO 유림 : 플레이어 페이지 추가

    m_InvenBtn = CUIButton::Create(m_GraphicDev,3,2);
    m_PlayerBtn = CUIButton::Create(m_GraphicDev,3,2);
    
    invenTransformCom = dynamic_cast<CTransform*>(m_InventoryPage->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    invenBtnTransformCom = dynamic_cast<CTransform*>(m_InvenBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    playerBtnTransformCom = dynamic_cast<CTransform*>(m_PlayerBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    //m_InvenBtn->Get_Transform()->Set_Pos({ 0.f, 0.f, 0.f });   
    //m_PlayerBtn->Get_Transform()->Set_Pos({ -250.f, 0.f, 0.f });
    invenTransformCom->Set_Pos(m_ClosedX, 0.f, 0.f);
    m_InvenBtn->SetCloneTexAspectScale({ 0.25f, 0.25f, 0.f });   
    m_PlayerBtn->SetCloneTexAspectScale({ 0.25f, 0.25f, 0.f });

    m_InvenBtn->SetOnClick([this]() {
        m_Current = UITabCategory::INVENTORY;
        });

    m_PlayerBtn->SetOnClick([this]() {
        m_Current = UITabCategory::PLAYER;
        });

    return S_OK;
} 

_int CUITabMenu::Update_GameObject(const _float& timeDelta)
{
   int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_TAB))
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
    _vec2 invenOffset = { 0.f, 0.f };
    _vec2 invenButtonOffset = { -550.f, -100.f };
    _vec2 playerButtonOffset = { -200.f, -100.f };

    if(m_IsOpen||m_IsAnimating)
    {
        invenBtnTransformCom->Set_Pos(pos.x + invenButtonOffset.x, pos.y + invenButtonOffset.y, 0.f);
        m_InvenBtn->Update_GameObject(timeDelta);
        playerBtnTransformCom->Set_Pos(pos.x + playerButtonOffset.x, pos.y + playerButtonOffset.y, 0.f);
        m_PlayerBtn->Update_GameObject(timeDelta);
        cout << invenBtnTransformCom->Get_Pos().x << "\t" << invenBtnTransformCom->Get_Pos().y << endl;
        cout << playerBtnTransformCom->Get_Pos().x << "\t" << playerBtnTransformCom->Get_Pos().y << endl;
        switch (m_Current)
        {
        case UITabCategory::INVENTORY:
            invenTransformCom->Set_Pos(pos.x + invenOffset.x, pos.y + invenOffset.y, 0.f);
            m_InventoryPage->Update_GameObject(timeDelta);
            break;
        }
    }

    return exit;
}

HRESULT CUITabMenu::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });
}

void CUITabMenu::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_IsOpen && !m_IsAnimating)
      return;

    switch(m_Current)
    {
    case UITabCategory::INVENTORY:
        m_InventoryPage->Render_GameObject();
        break;

    case UITabCategory::PLAYER:
        break;
    }

    m_InvenBtn->Render_GameObject();
    m_PlayerBtn->Render_GameObject();

}
void CUITabMenu::SetInventory(CInventory* inventory)
{
    m_Inventory = inventory;
    m_InventoryPage->SetInventory(m_Inventory);
    if (!m_Inventory)
        return;
}
