#include "pch.h"
#include "CCameraController.h"

CCameraController::CCameraController(DEVICE graphicDev)
    : CComponent(graphicDev)
{ }

CCameraController::CCameraController(const CCameraController& rhs)
    : CComponent(rhs)
{ }

CCameraController::~CCameraController()
{ }

HRESULT CCameraController::Ready_CameraController()
{
    return S_OK;
}

void CCameraController::Initialize_Component()
{
    
}

_int CCameraController::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraController::LateUpdate_Component()
{ }

CCameraController* CCameraController::Create(DEVICE graphicDev)
{
    auto controller = new CCameraController(graphicDev);

    if (FAILED(controller->Ready_CameraController()))
    {
        Safe_Release(controller);
        MSG_BOX("CameraController Create Failed");
        return nullptr;
    }

    return controller;
}

CComponent* CCameraController::Clone()
{
    return new CCameraController(*this);
}

void CCameraController::Free()
{
    //TODO 캐싱해서 사용한 포인터는 nullptr로 반납

    CComponent::Free();
}
