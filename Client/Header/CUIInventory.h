#pragma once
#include "CUI.h"

class CUIInventoryCell;
class CInventory;

class CUIInventory : public CUI
{
private:
   explicit CUIInventory(DEVICE pGraphicDev);
   explicit CUIInventory(const CUIInventory& rhs);
   ~CUIInventory() override;

public:
   void    OnBeginOverlap(CCollider* self, CCollider* other) override;
   void    OnEndOverlap(CCollider* self, CCollider* other) override;
    void    Render_Editor() override;
   HRESULT Ready_GameObject() override;
   _int    Update_GameObject(const _float& timeDelta) override;
   void    LateUpdate_GameObject(const _float& timeDelta) override;
   void    Render_GameObject() override;
   void     SetInventory(CInventory* inventory) { m_Inventory = inventory; }
   void CreateCells();
   HRESULT  Add_Component();

   static CUIInventory* Create(DEVICE graphicDev);

protected:
   void Free() override;

private:
    int     m_Cols;                         // 한 줄에 몇 칸?
    float   m_CellWidth;                    // 셀 가로 크기
    float   m_CellHeight;                   // 셀 세로 크기
    CInventory* m_Inventory;
    vector<CUIInventoryCell*> m_Cells;

};

