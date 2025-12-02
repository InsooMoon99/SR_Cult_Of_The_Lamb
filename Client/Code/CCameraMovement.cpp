#include "pch.h"
#include "CCameraMovement.h"

#include "CCameraController.h"
#include "CTransform.h"

CCameraMovement::CCameraMovement(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_TargetTransformCom(nullptr),

      m_PivotOffset(initOffset),
      m_TargetDistance(initDist),
      m_CamPitch(D3DXToRadian(initPitch)),
      m_CamYaw(D3DXToRadian(initYaw)),
      m_CurrentDistance(0.f),
      m_LagSpeed(5.f),
      m_ZoomSpeed(5.f)
{ }

CCameraMovement::CCameraMovement(const CCameraMovement& rhs)
    : CComponent(rhs),

      m_TargetTransformCom(nullptr),

      m_PivotOffset(rhs.m_PivotOffset),
      m_TargetDistance(rhs.m_TargetDistance),
      m_CamPitch(rhs.m_CamPitch),
      m_CamYaw(rhs.m_CamYaw),
      m_CurrentDistance(rhs.m_CurrentDistance),
      m_LagSpeed(rhs.m_LagSpeed),
      m_ZoomSpeed(rhs.m_ZoomSpeed)
{ }

CCameraMovement::~CCameraMovement()
{ }

HRESULT CCameraMovement::Ready_CameraMovement()
{
    return S_OK;
}

_int CCameraMovement::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraMovement::LateUpdate_Component()
{ }

void CCameraMovement::Reset_Camera(const _vec3& pivotOffset,
                                   _float       dist,
                                   _float       pitch,
                                   _float       yaw)
{
    m_PivotOffset    = pivotOffset;
    m_TargetDistance = dist;
    m_CamPitch       = pitch;
    m_CamYaw         = yaw;

    Clamp_Pitch();

    if (m_TargetTransformCom)
        m_CurrentPivotPos = m_TargetTransformCom->Get_Pos() + m_PivotOffset;
    else
        m_CurrentPivotPos = _vec3(0.f, 0.f, 0.f);

    m_CurrentDistance = m_TargetDistance;

    Update_Orbit(0.f);
}

void CCameraMovement::Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta)
{
    NULL_CHECK(m_TransformCom);

    _vec3 vMoveDir(0.f, 0.f, 0.f);

    if (dir.z != 0.f)
        vMoveDir += m_TransformCom->Get_Look() * dir.z;

    if (dir.x != 0.f)
        vMoveDir += m_TransformCom->Get_Right() * dir.x;

    if (dir.y != 0.f)
        vMoveDir += m_TransformCom->Get_Up() * dir.y;

    if (D3DXVec3Length(&vMoveDir) > 0.f) // 0벡터 정규화 방지
        D3DXVec3Normalize(&vMoveDir, &vMoveDir);

    m_TransformCom->Move_Pos(vMoveDir, timeDelta, speed);
}

void CCameraMovement::RotateX_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamPitch += angleDelta;
    Clamp_Pitch();

    m_TransformCom->Set_Rotation(_vec3(m_CamPitch, m_CamYaw, 0.f));
}

void CCameraMovement::RotateY_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamYaw += angleDelta;

    m_TransformCom->Set_Rotation(_vec3(m_CamPitch, m_CamYaw, 0.f));
}

void CCameraMovement::Update_Orbit(const _float& timeDelta)
{
    NULL_CHECK(m_TransformCom);
    NULL_CHECK(m_TargetTransformCom);

    _vec3 targetPos = m_TargetTransformCom->Get_Pos() + m_PivotOffset;

    _matrix matRot;
    D3DXMatrixRotationYawPitchRoll(&matRot, m_CamYaw, m_CamPitch, 0.f);

    _vec3 offsetDist(0.f, 0.f, -m_TargetDistance);
    D3DXVec3TransformCoord(&offsetDist, &offsetDist, &matRot);

    _vec3 finalPos = targetPos + offsetDist;

    m_TransformCom->Set_Pos(finalPos);

    if (m_TargetDistance > 0.01f)
        m_TransformCom->LookAt(targetPos);
    else
        m_TransformCom->Set_Rotation(_vec3(m_CamPitch, m_CamYaw, 0.f));
}

void CCameraMovement::Clamp_Pitch()
{
    constexpr _float rotXLimit = D3DXToRadian(89.f);

    if (m_CamPitch > rotXLimit) m_CamPitch = rotXLimit;
    else if (m_CamPitch < -rotXLimit) m_CamPitch = -rotXLimit;
}

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
    m_TargetTransformCom = nullptr;

    CComponent::Free();
}
