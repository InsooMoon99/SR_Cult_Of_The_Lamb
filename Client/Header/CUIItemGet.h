#pragma once
#include "CUI.h"
#include "ItemInstance.h"

typedef struct UIItem
{
    int   itemId;
    float remainTime;
    int   count;
};

class CUIItemGet : public CUI
{
private:    
    explicit CUIItemGet(DEVICE graphicDev);
    explicit CUIItemGet(const CUIItemGet& rhs);
    ~CUIItemGet() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta);
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    void    Add_UIItem(ItemInstance* iteminstance);


    static CUIItemGet* Create(DEVICE graphicDev);
    void    Render_Editor() override;

private:
    UIItem      m_UIItem;
    ItemInstance* m_ItemInstance;
    list<UIItem> m_UIItems;

private:
    void Free() override;


};


