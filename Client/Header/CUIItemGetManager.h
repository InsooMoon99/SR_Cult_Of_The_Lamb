#pragma once
#include "CGameObject.h"

class CUIItemGet;

typedef struct UIItem
{
    int   itemId;
    float remainTime;
    int   count;
};
class CUIItemGetManager : public CGameObject
{
private:
    explicit CUIItemGetManager(DEVICE graphicDev);
    explicit CUIItemGetManager(const CUIItemGetManager& rhs);
    ~CUIItemGetManager() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

protected:
    void Free() override;

private:
    CUIItemGet* m_UIItemGet;
    list<UIItem> m_UIItems;

};


