#include "pch.h"
#include "CUIInventory.h"

#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CInventory.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "CFontMgr.h"
#include "CTimerMgr.h"
#include "CUIInventoryCell.h"

CUIInventory::CUIInventory(DEVICE pGraphicDev)
    : CUI(pGraphicDev),
      m_Cols(6),
      m_CellWidth(100.f),
      m_CellHeight(150.f),
      m_IsOpen(false),
      m_Inventory(nullptr)
{}

CUIInventory::CUIInventory(const CUIInventory& rhs)
    : CUI(rhs),
      m_Cols(6),
      m_CellWidth(100.f),
      m_CellHeight(150.f),
      m_IsOpen(false),
      m_Inventory(nullptr)
{}

CUIInventory::~CUIInventory()
{}

HRESULT CUIInventory::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(-WINCX / 5, 0.f, 1.f);
    m_TransformCom->Set_Scale(420.f, 400.f, 1.f);

    return S_OK;
}

_int CUIInventory::Update_GameObject(const _float& timeDelta)
{
    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);
    

    if (m_IsOpen == false && CDInputMgr::GetInstance()->KeyDown(DIK_TAB))
    {
        m_IsOpen = true;    
    }
    
    else if (m_IsOpen == true && CDInputMgr::GetInstance()->KeyDown(DIK_TAB))
        m_IsOpen = false;

    const vector<InventorySlot>& slots = m_Inventory->GetVector();
    
    for (int i = 0; i < slots.size(); i++)
    {
        auto& slot = slots[i];

        m_Cells[i]->SetInventoryCell(slot.itemInst, slot.count);
        m_Cells[i]->Update_GameObject(CTimerMgr::GetInstance()->Get_TimeDelta());

    }


    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIInventory::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIInventory::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    _vec2 pos{ m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };
    _vec2 material = { WINCX / 12, WINCY / 2 - 170 };
    _vec2 food     = { WINCX / 12, WINCY / 2 };

    if (m_IsOpen)
    {
        //인벤토리 배경 그리기
        m_TextureCom->Set_Texture(0);
        m_BufferCom->Render_Buffer();
            
        CFontMgr::GetInstance()->Render_Font(L"LaptureDisplay-Regular",
            L"재화",
            &material,
            D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
       
       
        CFontMgr::GetInstance()->Render_Font(L"LaptureDisplay-Regular",
            L"음식",
            &food,
            D3DXCOLOR(0.5f, 0.5, 0.5f, 1.f));

        const vector<InventorySlot>& slots = m_Inventory->GetVector();
        int slotCount = slots.size();
        for (int i = 0; i < slotCount; i++)
        {
                        m_Cells[i]->Render_GameObject();
        }
    }
}

void CUIInventory::SetInventory(CInventory* inventory)
{
     m_Inventory = inventory;

     if (!m_Inventory)
         return;

     CreateCells();

    SetPositions();
}

void CUIInventory::CreateCells()
{
    int rows = 2;
    int cols = 6;

    for (int i = 0; i < rows*cols; i++)
    {
        CUIInventoryCell* cell = CUIInventoryCell::Create(m_GraphicDev);
        m_Cells.push_back(cell);
    }
}


void CUIInventory::SetPositions()
{
    const vector<InventorySlot>& slots = m_Inventory->GetVector();

    int slotCount = slots.size();

    int materialRenderCount = 0;

    int startOffset = -500.f;

    for (int i = 0; i < slotCount / 2; i++)
    {
            int col = materialRenderCount % m_Cols;

            float x = startOffset + (float)col * m_CellWidth;

            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            transform->Set_Pos(x, 100.f, 0.f);

            materialRenderCount++;
    }
    int foodRenderCount = 0;

    for (int i = slotCount / 2; i < slotCount; i++)
    {
            int col = foodRenderCount % m_Cols;

            float x = startOffset + (float)col * m_CellWidth;

            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            transform->Set_Pos(x, -70.f, 0.f);

            foodRenderCount++;
    }


}

HRESULT CUIInventory::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEXTURE, L"UI_Inven");
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEXTURE, m_TextureCom });

    return S_OK;
}

void CUIInventory::Check_If_Empty()
{
    const vector<InventorySlot>& slots = m_Inventory->GetVector();

    m_IsMaterialEmpty = true;

    for(int i = 0; i < slots.size()/2; i++)
    {
        if(slots[i].itemInst.itemId!=-1)
        {
            m_IsMaterialEmpty = false;
            break;
        }
        
    }
    for (int i = slots.size() / 2; i < slots.size(); i++)
    {
        if (slots[i].itemInst.itemId != -1)
        {
            m_IsMaterialEmpty = false;
            break;
        }

    }
    //for(auto& slot :slots)
    //{   if (slot.itemInst.itemId)
    //    if (slot.itemInst.itemId != -1)
    //        m_IsEmpty = false;
    //    break;
    //}
}

void CUIInventory::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUIInventory::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

void CUIInventory::Render_Editor()
{
    CUI::Render_Editor();
}

CUIInventory* CUIInventory::Create(DEVICE graphicDev)
{
    CUIInventory* inventoryUI = new CUIInventory(graphicDev);

    if (FAILED(inventoryUI->Ready_GameObject()))
    {
        Safe_Release(inventoryUI);
        MSG_BOX("inventoryUI Created Failed");
        return nullptr;
    }
    return inventoryUI;
}

void CUIInventory::Free()
{
    for (auto& cell : m_Cells)
    {
        Safe_Release(cell);
    }

    m_Cells.clear();

    CUI::Free();
}
