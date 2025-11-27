#pragma once
#include "CUI.h"
#include "CInventory.h"
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

    public: 
        virtual HRESULT Ready_GameObject() override;
        virtual _int    Update_GameObject(const _float& timeDelta) override;
        virtual void    Render_GameObject() override;
        void LateUpdate_GameObject(const _float& timeDelta) override;

        void SetInventory(CInventory* inventory);
        HRESULT Add_Component();
        static CUITabMenu* Create(DEVICE graphicDev);
        void ChangePage(UITabCategory newTab);

    protected:
        void Free() override;

    private:
        UITabCategory   m_Current;
        CUIButton* m_InvenBtn;
        CUIButton* m_PlayerBtn;
        CInventory* m_Inventory;
        CUIInventory* m_InventoryPage;
        CUIPlayerPage* m_PlayerPage;

        CTransform* m_InvenTransformCom;
        CTransform* m_InvenBtnTransformCom;
        CTransform* m_PlayerBtnTransformCom;
        CTransform* m_PlayerPageTransformCom;

        float m_TargetX;
        float m_ClosedX;
        float m_Speed;
        bool m_IsOpen;
        bool m_IsAnimating;
};

