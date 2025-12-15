#pragma once
#include "CRunLootLog.h"
#include "CUI.h"
class CUIInventoryCell;
class CUIIcon;

class CUI_DungeonLootPanel : public CUI
{
private:
    explicit CUI_DungeonLootPanel(DEVICE graphicDev);
    explicit CUI_DungeonLootPanel(const CUI_DungeonLootPanel& rhs);
    ~CUI_DungeonLootPanel() override;
public:
    void    Initialize_GameObject() override;
    void     SetPositions();
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    CreateCells();
    HRESULT Add_Component();
    void    Set_RunLootLog(CRunLootLog* runlootlog) { m_RunLootLog = runlootlog; }
    static CUI_DungeonLootPanel* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int     m_Cols;                         // 한 줄에 몇 칸?
    float   m_CellWidth;                    // 셀 가로 이동거리
    float   m_CellHeight;                   // 셀 세로 이동거리

    CUIIcon* m_DecoIcon1;
    CUIIcon* m_DecoIcon2;
    CUIIcon* m_DecoIcon3;

private:
    CUIIcon* m_ItemIcon;
    CUIIcon* m_TimerIcon;
    vector<CUIInventoryCell*> m_Cells;
    CRunLootLog* m_RunLootLog;

};


