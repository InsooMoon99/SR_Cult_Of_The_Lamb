#include "pch.h"
#include "CUI.h"
#include "CUIInventory.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CInventory.h"
#include "CUIInventoryCell.h"
#include "CRenderer.h"
#include "CTexture.h"
CUIInventory::CUIInventory(DEVICE pGraphicDev) : CUI(pGraphicDev), m_Cols(6), m_CellWidth(100.f),m_CellHeight(100.f)
{
}


CUIInventory::CUIInventory(const CUIInventory& rhs) : CUI(rhs), m_Cols(6) , m_CellWidth(100.f), m_CellHeight(100.f)
{
}

CUIInventory::~CUIInventory()
{

}

HRESULT CUIInventory::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    
    return S_OK;
}

_int CUIInventory::Update_GameObject(const _float& timeDelta)
{

    if (m_Cells.empty())
        CreateCells();

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);

    const vector<InventorySlot>& slots = m_Inventory->GetVector();

    for(int i = 0; i < slots.size(); i++)
    {
        auto& slot = slots[i];

        m_Cells[i]->SetInventoryCell(slot.itemInst, slot.count);
        m_Cells[i]->Update_GameObject(timeDelta);
    }

    return 0;
}

void CUIInventory::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIInventory::Render_GameObject()
{
    CUI::Render_GameObject();

    const vector<InventorySlot>& slots = m_Inventory->GetVector();
    int slotCount = slots.size();

    int materialRenderCount = 0;

    for (int i = 0; i < slotCount / 2; i++)
    {
        if (slots[i].itemInst.itemId != -1)
        {
            int col = materialRenderCount % m_Cols;

            float x = (float)col * m_CellWidth;

            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
            transform->Set_Pos(x, 100.f, 0.f);

            m_Cells[i]->Render_GameObject();

            materialRenderCount++;
        }

    }
    int foodRenderCount = 0;

    for (int i = slotCount / 2; i < slotCount; i++)
    {
        if (slots[i].itemInst.itemId != -1)
        {
            int col = foodRenderCount % m_Cols;

            float x = (float)col * m_CellWidth;

            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
            transform->Set_Pos(x, 200.f, 0.f);

            m_Cells[i]->Render_GameObject();

            foodRenderCount++;
        }

    }
}

void CUIInventory::CreateCells()
{
    const vector<InventorySlot>& slots = m_Inventory->GetVector();

    int slotCount = slots.size();

    m_Cells.reserve(slotCount);

    for (int i = 0; i < slotCount; i++)
    {
        CUIInventoryCell* cell = CUIInventoryCell::Create(m_GraphicDev);
        m_Cells.push_back(cell);
    }
}

HRESULT CUIInventory::Add_Component()
{
   // // buffer
   // m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
   // NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
   //
   // m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });
   //
   // // transform
   // m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
   // NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
   //
   // m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });
   //
   // // texture
   // m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_INVEN);
   // NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
   //
   // m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_CELL, m_TextureCom });
   //
   //
   return S_OK;
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
    CUI::Free();
}
