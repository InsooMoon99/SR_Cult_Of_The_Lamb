#pragma once
#include "CRenderObject.h"

BEGIN(Engine)
class CBoxCollider;
END

// 건축물 오브젝트의 베이스
class CBuildingBase : public CRenderObject
{
protected:
    explicit CBuildingBase(DEVICE graphicDev);
    explicit CBuildingBase(const CBuildingBase& rhs);
    ~CBuildingBase() override;

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

protected:
    HRESULT Add_Component() override;

protected:
    void Free() override;

protected:
    Engine::CBoxCollider* m_BoxColCom = nullptr;
};
