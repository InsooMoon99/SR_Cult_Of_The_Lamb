#pragma once
#include "CUI.h"
class CInventory;
class CUIPlayerPage;
class CUIInventory;
class CUIButton;


enum class UITabCategory
{
    INVENTORY,
    PLAYER
};

class CUITabMenu : public CUI
{
    public:
        explicit CUITabMenu(DEVICE graphicDev);
        explicit CUITabMenu(const CUI& rhs);
        ~CUITabMenu() override;


        virtual HRESULT Ready_GameObject() override;
        virtual _int    Update_GameObject(const _float& timeDelta) override;
        virtual void    Render_GameObject() override;
        void SetInventory(CInventory* inventory);
        static CUITabMenu* Create(DEVICE graphicDev);
        HRESULT Add_Component();

private:
        UITabCategory   m_Current;

        CUIButton* m_InvenBtn;
        CUIButton* m_PlayerBtn;
        CInventory* m_Inventory;
        CUIInventory* m_InventoryPage;

        CTransform* invenTransformCom;
        CTransform* invenBtnTransformCom;
        CTransform* playerBtnTransformCom;

        float m_TargetX;
        float m_ClosedX;
        float m_Speed;
        bool m_IsOpen;
        bool m_IsAnimating;

        //CUIPlayerPage* m_PlayerPage = nullptr;

};

