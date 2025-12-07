#pragma once
#include "CUI.h"

class CPlayerSkill;

class CUICharging : public CUI
{
private:
    explicit CUICharging(DEVICE graphicDev);
    explicit CUICharging(const CUICharging& rhs);
    ~CUICharging() override;

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    void    Render_GameObject() override;
    int     Update_GameObject(const _float& timeDelta) override;
    HRESULT Add_Component();

    void    Set_PlayerSkill(CPlayerSkill* playerSkill) { m_PlayerSkill = playerSkill; }
    static CUICharging* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CPlayerSkill* m_PlayerSkill;
    float         m_ChargeRatio;
    
};


