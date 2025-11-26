#pragma once
#include "CUI.h"
class CUIPlayerPage : public CUI
{
private:
    explicit CUIPlayerPage(DEVICE pGraphicDev);
    explicit CUIPlayerPage(const CUIPlayerPage& rhs);
    ~CUIPlayerPage();

public:
    HRESULT        Add_Component();
    HRESULT        Ready_GameObject() override;
    _int           Update_GameObject(const _float& timeDelta) override;
    void           LateUpdate_GameObject(const _float& timeDelta) override;
    void           Render_GameObject() override;

    static CUIPlayerPage* Create(DEVICE graphicDev);
    void                  Free() override;
};

