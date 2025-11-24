#pragma once
#include "CUI.h"

class CUIIcon;
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
   void     SetInventory(CInventory* inventory);
   void    SetPositions();
   HRESULT  Add_Component();

    void Check_If_Empty();
    void CreateCells();
    void On_Item_Gained();

    static CUIInventory* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int     m_Cols;                         // 한 줄에 몇 칸?
    float   m_CellWidth;                    // 셀 가로 이동거리
    float   m_CellHeight;                   // 셀 세로 이동거리
    bool    m_IsOpen;
    bool    m_IsMaterialEmpty;
    bool    m_IsFoodEmpty;

    CInventory* m_Inventory;

    vector<CUIInventoryCell*> m_Cells;
};
