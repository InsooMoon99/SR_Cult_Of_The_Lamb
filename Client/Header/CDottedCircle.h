#pragma once
#include "CBuildingBase.h"

class CDottedCircle : public CBuildingBase
{
private:
    explicit CDottedCircle(DEVICE graphicDev);
    explicit CDottedCircle(const CDottedCircle& rhs);
    ~CDottedCircle() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    public:
    void Setup_Construction(BUILDING_TYPE type);

private:
    HRESULT Add_Component() override;

public:
    static CDottedCircle* Create(DEVICE graphicDev, _vec3 scale = { 1.f, 1.f, 1.f });

private:
 void Free() override;


private:
    BUILDING_TYPE m_TargetBuildingType = BUILDING_END;
    _float        m_fProgress          = 0.f;
};
