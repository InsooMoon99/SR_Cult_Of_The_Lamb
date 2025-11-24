#pragma once
#include "CGameObject.h"

class CBossPhase1 :
    public CGameObject
{
private:
    explicit CBossPhase1(DEVICE graphicDev);
    explicit CBossPhase1(const CBossPhase1& rhs);
    ~CBossPhase1();
public:
     HRESULT Ready_GameObject();
     _int    Update_GameObject(const _float& timeDelta);
     void    LateUpdate_GameObject(const _float& timeDelta);
     void    Render_GameObject();


public:
    virtual void OnBeginOverlap(CCollider* self, CCollider* other) {}
    virtual void OnEndOverlap(CCollider* self, CCollider* other) {}

public:
    static CBossPhase1* Create(DEVICE graphicDev);

private:
    void Free() override;
    
};

