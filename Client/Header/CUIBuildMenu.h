#pragma once
#include "CUI.h"

class CUIBuildCell;
class CUIIcon;

class CUIBuildMenu : public CUI
{
private:
    explicit CUIBuildMenu(DEVICE graphicDev);
    explicit CUIBuildMenu(const CUI& rhs);
    ~CUIBuildMenu() override;

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;
    void LateUpdate_GameObject(const _float& timeDelta) override;

    HRESULT Add_Component();
    void Initialize();
    void Check_Hovered();

    static CUIBuildMenu* Create(DEVICE graphicDev);
    
protected:
    void Free() override;

private:
    CUIBuildCell* m_CookingFireCell; // 성지 셀
    CUIBuildCell* m_TempleCell; // 사원 셀
    vector<CUIBuildCell*> m_BuildCells;

    float m_TargetX;
    float m_ClosedX;
    float m_Speed;
    bool m_IsOpen;
    bool m_IsAnimating;
};

