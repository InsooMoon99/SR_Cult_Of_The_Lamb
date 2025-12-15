#pragma once
#include "CScene.h"

class CUIButton;

class CMainMenu : public CScene
{
private:
    explicit CMainMenu(DEVICE graphicDev);
    ~CMainMenu() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;
    HRESULT Ready_Light() override;

    void    RenderFont(const wstring& font, const wstring& text, CUIButton* btt);

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

private:
    CUIButton* bt;
    CUIButton* bt1;
    CUIButton* bt2;
    CUIButton* bt3;

public:
    static CMainMenu* Create(DEVICE graphicDev);

private:
    void Free() override;
};
