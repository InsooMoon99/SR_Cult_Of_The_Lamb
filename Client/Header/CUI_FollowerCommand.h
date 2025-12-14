#pragma once
#include "CUI.h"

namespace Engine
{
class CBoxCollider;
class CAnimator;
}

class CUIButton;

class CUI_FollowerCommand :public CUI
{
private:
    CUI_FollowerCommand(DEVICE graphicDev);
    CUI_FollowerCommand(const CUI_FollowerCommand& rhs);
    ~CUI_FollowerCommand() override;

public:
    void    Initialize_GameObject() override;
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    _bool   Get_Active() { return m_Active; }
    static CUI_FollowerCommand* Create(DEVICE graphicDev);

    HRESULT Add_Component();

protected:
    void Free() override;

private:
    CUIButton* m_PrayButton;
    CUIButton* m_MiningButton;
    CUIButton* m_AxeButton;
};


