#pragma once

#include "CGameObject.h"

BEGIN(Engine)
class CTransform;
class CBoxCollider;
END

class CWall : public CGameObject
{
private:
    explicit CWall(DEVICE graphicDev);
    explicit CWall(const CWall& rhs);
    virtual ~CWall();

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

private:
    HRESULT Add_Component();

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;

    static CWall* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CTransform*     m_TransformCom;
    CBoxCollider*   m_BoxCollider;
};

