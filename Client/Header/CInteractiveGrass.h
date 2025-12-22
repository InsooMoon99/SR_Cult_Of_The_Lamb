#pragma once
#include "CWorldInteractive.h"

class CInteractiveGrass : public CWorldInteractive
{
protected:

    explicit CInteractiveGrass(DEVICE graphicDev);
    explicit CInteractiveGrass(const CInteractiveGrass& rhs);
    virtual ~CInteractiveGrass();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    OnInteractiveDamaged() override;

protected:
    virtual HRESULT Add_Component();

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    static CInteractiveGrass* Create(DEVICE graphicDev);

protected:

    void Free() override;

private:
    _int m_DeadTexIndex;
    _int m_CurTexIndex;
};

