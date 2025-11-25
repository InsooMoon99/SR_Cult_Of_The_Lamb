#pragma once
#include "CUI.h"

class CUIButton;
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
void    Check_Hovered();
void CreateCells();
    static CUIInventory* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int     m_Cols;                         // 한 줄에 몇 칸?
    float   m_CellWidth;                    // 셀 가로 이동거리
    float   m_CellHeight;                   // 셀 세로 이동거리
    bool    m_IsOpen;
    float   m_ClosedX;                      // 인벤토리 닫혀 있을 때의 X 좌표
    float   m_TargetX;                      // 인벤토리 열려 있을 때의 X 좌표
    bool    m_IsAnimating;                  // 애니메이션 진행 중인가?
    float   m_Speed;                        // 애니메이션 속도
    CUIButton* m_Button;        
    CInventory* m_Inventory;
    vector<CUIInventoryCell*> m_Cells;
};
