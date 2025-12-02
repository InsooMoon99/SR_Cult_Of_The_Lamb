#include "pch.h"
#include "CCameraController.h"

#include "CCameraCalc.h"
#include "CCameraMovement.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

CCameraController::CCameraController(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerCamCalcCom(nullptr),
      m_OwnerCamMovementCom(nullptr),

      m_CamMode(CAM_FREE),
      m_ViewType(VIEW_FPS),
      m_ProjType(PROJ_PERSPECTIVE),

      m_Offset(0.f, 6.f, -10.f)
{ }

CCameraController::CCameraController(const CCameraController& rhs)
    : CComponent(rhs),

      m_OwnerCamCalcCom(nullptr),
      m_OwnerCamMovementCom(nullptr),

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
{
    m_OwnerCamCalcCom     = dynamic_cast<CCameraCalc*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CALC));
    m_OwnerCamMovementCom = dynamic_cast<CCameraMovement*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMPONENT_END));

    if (nullptr == m_OwnerCamMovementCom)
    { }
}

_int CCameraController::Update_Component(const _float& timeDelta)
{
    if (nullptr == m_OwnerCamCalcCom || nullptr == m_OwnerCamMovementCom)
        return 0;

    switch (m_CamMode)
    {
    case CAM_TARGET:
        if (false == m_OwnerCamMovementCom->Update_TargetMode(timeDelta))
        {
            m_OwnerCamMovementCom->Set_Target(nullptr);
            m_CamMode = CAM_FREE;
        }
        break;

    case CAM_FREE:
        Control_FreeMode(timeDelta);
        break;

    default: break;
    }

    return 0;
}

void CCameraController::LateUpdate_Component()
{ }

void CCameraController::Set_ProjType(PROJ_TYPE projType)
{
    m_ProjType = projType;
}

void CCameraController::Set_CamTarget(Engine::CTransform* targetTransform)
{
    NULL_CHECK(m_OwnerCamMovementCom);

    if (m_TargetTransformCom != targetTransform)
        m_TargetTransformCom = targetTransform;

    if (nullptr != m_TargetTransformCom)
    {
        m_CamMode = CAM_TARGET;
        m_OwnerCamMovementCom->Set_Target(targetTransform);
        m_OwnerCamMovementCom->Reset_Camera();
    }
    else
    {
        m_CamMode = CAM_FREE;
        m_OwnerCamMovementCom->Set_Target(nullptr);
    }
}

void CCameraController::Control_FreeMode(const _float& timeDelta)
{
    //TODO: 여기서는 방향만 지정하고, 실제 움직임은 CCameraMovement의 함수를 호출하는 식으로 하자

    if (nullptr == m_TransformCom || nullptr == m_OwnerCamMovementCom)
        return;

    auto         inputMgr = CDInputMgr::GetInstance();
    const _float speed    = 10.f;
    _vec3        dir      = { 0.f, 0.f, 0.f };

    if (inputMgr->Get_DIKeyState(DIK_LSHIFT) & 0x80)
    {
        // Y축 회전
        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(_vec3(0.f, 1.f, 0.f), D3DXToRadian(-180), timeDelta);

        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(_vec3(0.f, 1.f, 0.f), D3DXToRadian(180), timeDelta);

        // X축 회전
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(_vec3(1.f, 0.f, 0.f), D3DXToRadian(-180), timeDelta);

        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(_vec3(1.f, 0.f, 0.f), D3DXToRadian(180), timeDelta);
    }
    else
    {
        // 앞뒤
        dir = m_TransformCom->Get_Look();
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_OwnerCamMovementCom->Move_FreeMode(dir, speed, timeDelta);
        }

        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_OwnerCamMovementCom->Move_FreeMode(dir, -speed, timeDelta);
        }

        // 좌우
        dir = m_TransformCom->Get_Right();
        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_OwnerCamMovementCom->Move_FreeMode(dir, speed, timeDelta);
        }

        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_OwnerCamMovementCom->Move_FreeMode(dir, -speed, timeDelta);
        }
    }

    // 상하
    dir = m_TransformCom->Get_Up();
    if (inputMgr->Get_DIKeyState(DIK_P) & 0x80)
    {
        D3DXVec3Normalize(&dir, &dir);
        m_OwnerCamMovementCom->Move_FreeMode(dir, speed, timeDelta);
    }

    if (inputMgr->Get_DIKeyState(DIK_SEMICOLON) & 0x80)
    {
        D3DXVec3Normalize(&dir, &dir);
        m_OwnerCamMovementCom->Move_FreeMode(dir, -speed, timeDelta);
    }
}

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
