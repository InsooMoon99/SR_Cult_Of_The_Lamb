#include "pch.h"
#include "CCameraMovement.h"

CCameraMovement::CCameraMovement(DEVICE graphicDev)
    : CComponent(graphicDev)
{ }

CCameraMovement::CCameraMovement(const CCameraMovement& rhs)
    : CComponent(rhs)
{ }

CCameraMovement::~CCameraMovement()
{ }

HRESULT CCameraMovement::Ready_CameraMovement()
{
    return S_OK;
}

void CCameraMovement::Initialize_Component()
{ }

_int CCameraMovement::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraMovement::LateUpdate_Component()
{ }

CCameraMovement* CCameraMovement::Create(DEVICE graphicDev)
{
    auto camMovement = new CCameraMovement(graphicDev);

    if (FAILED(camMovement->Ready_CameraMovement()))
    {
        Safe_Release(camMovement);
        MSG_BOX("CameraMovement Create Failed");
        return nullptr;
    }

    return camMovement;
}

CComponent* CCameraMovement::Clone()
{
    return new CCameraMovement(*this);
}

void CCameraMovement::Free()
{
    CComponent::Free();
}
