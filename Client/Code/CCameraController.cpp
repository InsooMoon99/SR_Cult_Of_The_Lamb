#include "pch.h"
#include "CCameraController.h"

CCameraController::CCameraController(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_CamCalc(nullptr),
      m_CamMovement(nullptr),

      m_CamMode(CAM_FREE),
      m_ViewType(VIEW_FPS),
      m_ProjType(PROJ_PERSPECTIVE),

      m_Offset(0.f, 6.f, -10.f)
{ }

CCameraController::CCameraController(const CCameraController& rhs)
    : CComponent(rhs),

      m_CamCalc(nullptr),
      m_CamMovement(nullptr),

      m_CamMode(rhs.m_CamMode),
      m_ViewType(rhs.m_ViewType),
      m_ProjType(rhs.m_ProjType),

      m_Offset(rhs.m_Offset)
{ }

CCameraController::~CCameraController()
{ }

HRESULT CCameraController::Ready_CameraController()
{
    return S_OK;
}

void CCameraController::Initialize_Component()
{ }

_int CCameraController::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraController::LateUpdate_Component()
{ }

CCameraController* CCameraController::Create(DEVICE graphicDev)
{
    auto camController = new CCameraController(graphicDev);

    if (FAILED(camController->Ready_CameraController()))
    {
        Safe_Release(camController);
        MSG_BOX("CameraController Create Failed");
        return nullptr;
    }

    return camController;
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
