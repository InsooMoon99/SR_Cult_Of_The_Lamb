#pragma once
#include "CUI.h"
class CUIIcon;

class CUIWeaponExplain : public CUI
{
private:
    explicit CUIWeaponExplain(DEVICE graphicDev);
    explicit CUIWeaponExplain(const CUIWeaponExplain& rhs);
    ~CUIWeaponExplain();

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    void    Set_ItemID(int ID) { m_ItemID = ID; }
    void   Set_Active(_bool tf) { m_Active = tf; }
    static CUIWeaponExplain* Create(DEVICE graphicDev);
protected:
    void Free() override;
private:
    int m_ItemID;
};


