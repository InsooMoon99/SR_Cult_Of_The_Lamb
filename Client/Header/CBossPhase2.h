#pragma once

#include "CMonsterBase.h"

class CBossPhase2 : public CMonsterBase
{
private:
    explicit CBossPhase2(DEVICE graphicDev);
    explicit CBossPhase2(const CBossPhase2& rhs);
    virtual ~CBossPhase2() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    void    Render_Editor() override;

public:
    static CBossPhase2* Create(DEVICE graphicDev);

protected:
    void    Animation_Setting() override;
    HRESULT Add_Component() override;

protected:
    void    Free() override;

private:

};

