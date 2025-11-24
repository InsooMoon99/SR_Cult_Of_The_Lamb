#pragma once
#include "CGameObject.h"

class CBossPhase1 : public CGameObject
{
private:
    explicit CBossPhase1(DEVICE graphicDev);
    explicit CBossPhase1(const CBossPhase1& rhs);
    ~CBossPhase1() override;
public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override {}
    void OnEndOverlap(CCollider* self, CCollider* other) override {}

public:
    static CBossPhase1* Create(DEVICE graphicDev);

private:
    void Free() override;
};
