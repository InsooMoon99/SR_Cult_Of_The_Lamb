#include "CSphereCollider.h"

#include "CBoxCollider.h"
#include "CRectCollider.h"

CSphereCollider::CSphereCollider(DEVICE graphicDev)
    : CCollider(graphicDev, COLLIDERTYPE::SPHERE)
{ }

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
    : CCollider(rhs), m_Radius(rhs.m_Radius)
{ }

CSphereCollider::~CSphereCollider()
{ }

HRESULT CSphereCollider::Ready_SphereCollider(DEVICE graphicDev)
{
    return S_OK;
}

_int CSphereCollider::Update_Component(const _float& timeDelta)
{
    _int exit = CCollider::Update_Component(timeDelta);

    return exit;
}

void CSphereCollider::LateUpdate_Component()
{
    CCollider::LateUpdate_Component();
}

void CSphereCollider::Render()
{
    //if (m_ShowDebug == false || m_BufferCom == nullptr)
    //    return;
}

bool CSphereCollider::CheckCollision(CCollider* other)
{
    switch (other->GetColliderType())
    {
    case COLLIDERTYPE::BOX:
        return CheckCollisionSphere2Box(this, static_cast<CBoxCollider*>(other));

    case COLLIDERTYPE::SPHERE:
        return CheckCollisionSphere2Sphere(this, static_cast<CSphereCollider*>(other));

    default:
        break;
    }

    return false;
}

CSphereCollider* CSphereCollider::Create(DEVICE graphicDev)
{
    auto sphere = new CSphereCollider(graphicDev);

    if (FAILED(sphere->Ready_SphereCollider(graphicDev)))
    {
        Safe_Release(sphere);
        MSG_BOX("SphereCollider Create Failed");
        return nullptr;
    }

    return sphere;
}

CComponent* CSphereCollider::Clone()
{
    return new CSphereCollider(*this);
}

void CSphereCollider::Free()
{
    CCollider::Free();
}
