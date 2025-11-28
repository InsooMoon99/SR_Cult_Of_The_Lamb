#pragma once
#include "CUI.h"

class CUICircle : public CUI
{
public:

    explicit CUICircle(DEVICE pGraphicDev);
    explicit CUICircle(const CUICircle& rhs);
    ~CUICircle() override;

    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;

    HRESULT Add_Component();

public:
    static CUICircle* Create(DEVICE graphicDev);

private:
    static int         m_nextID;
    int                 m_SkillCount=3;
protected:
    void Free() override;

};

