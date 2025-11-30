#pragma once
#include "CUI.h"

class CUITarotDescCard;
class CUIButton;
class CUITarotCard;

enum class STAGESTATE
{
    NONE,
    DIALOGUE,
    CARD,
    ENDING,
    THE_END
};

class CTarotStageController : public CUI
{
private:
    explicit CTarotStageController(DEVICE graphicDev);
    explicit CTarotStageController(const CTarotStageController& rhs);
    ~CTarotStageController() override;

public:
    void    Render_Editor() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    static CTarotStageController* Create(DEVICE graphicDev);
    void Create_Cards();

protected:
    void Free() override;

private:
    STAGESTATE             m_CurrentState;
    CUITarotCard*          m_TarotCard1;
    CUITarotCard*          m_TarotCard2;
    CUITarotDescCard*      m_ExplainCard1;
    CUITarotDescCard*      m_ExplainCard2;
};


