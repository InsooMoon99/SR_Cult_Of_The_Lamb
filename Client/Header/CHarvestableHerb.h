#pragma once
#include "CWorldInteractive.h"
class CHarvestableHerb :
    public CWorldInteractive
{
public:
    enum BERRY { BIGBERREY, SMALLBERRY, END_BERRY };
protected:

    explicit CHarvestableHerb(DEVICE graphicDev);
    explicit CHarvestableHerb(const CHarvestableHerb& rhs);
    virtual ~CHarvestableHerb();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    virtual HRESULT Add_Component();

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;


protected:


private:


public:
    static CHarvestableHerb* Create(DEVICE graphicDev);

protected:


    void Free() override;

};

