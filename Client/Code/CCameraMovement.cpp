#include "pch.h"
#include "CCameraMovement.h"

#include "CCameraController.h"
#include "CTransform.h"

CCameraMovement::CCameraMovement(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_TargetTransformCom(nullptr),

      m_CurrentPivotPos(0.f, 0.f, 0.f),
      m_CurrentDistance(0.f),

      m_ShakeAmp(0.f),
      m_ShakeDecay(0.f),
      m_ShakeFreq(0.f),
      m_ShakeTimeAcc(0.f),
      m_ShakeOffset(0.f, 0.f, 0.f)
{ }

CCameraMovement::CCameraMovement(const CCameraMovement& rhs)
    : CComponent(rhs),

      m_TargetTransformCom(nullptr),

      m_CamPreset(rhs.m_CamPreset),
      m_CurrentPivotPos(rhs.m_CurrentPivotPos),
      m_CurrentDistance(rhs.m_CurrentDistance),
      // ↓ 일시적 효과이기에 값 복사 필요 없음
      m_ShakeAmp(0.f),
      m_ShakeDecay(0.f),
      m_ShakeFreq(0.f),
      m_ShakeTimeAcc(0.f),
      m_ShakeOffset(0.f, 0.f, 0.f)
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

void CCameraMovement::Set_Target(CTransform* targetTransform)
{
    m_TargetTransformCom = targetTransform;
    Reset_Camera();
}

void CCameraMovement::Set_RotX(_float rotX)
{
    m_CamPreset.pitch = rotX;
    Clamp_Pitch();
}

void CCameraMovement::Apply_Preset(const CAM_PRESET& preset, _bool snap)
{
    m_CamPreset = preset;

    Clamp_Pitch();

    if (snap)
        Update_TargetMode(0.f);
}

void CCameraMovement::Start_CamShake(_float amplitude, _float decay, _float freq)
{
    m_ShakeAmp   = max(m_ShakeAmp, amplitude);
    m_ShakeDecay = decay;
    m_ShakeFreq  = freq;
}

void CCameraMovement::Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta)
{
    NULL_CHECK(m_TransformCom);

    _vec3 moveDir(0.f, 0.f, 0.f);

    if (dir.z != 0.f) moveDir += m_TransformCom->Get_Look() * dir.z;
    if (dir.x != 0.f) moveDir += m_TransformCom->Get_Right() * dir.x;
    if (dir.y != 0.f) moveDir += m_TransformCom->Get_Up() * dir.y;

    if (D3DXVec3Length(&moveDir) > 0.f) // 0벡터 정규화 방지
        D3DXVec3Normalize(&moveDir, &moveDir);

    m_TransformCom->Move_Pos(moveDir, timeDelta, speed);
}

void CCameraMovement::RotateX_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamPreset.pitch += angleDelta;
    Clamp_Pitch();
    m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));
}

void CCameraMovement::RotateY_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamPreset.yaw += angleDelta;
    m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));
}

_bool CCameraMovement::Update_TargetMode(const _float& timeDelta)
{
    NULL_CHECK_RETURN(m_TransformCom, false);
    NULL_CHECK_RETURN(m_TargetTransformCom, false);

    // 피벗 위치 계산
    _vec3 targetPos = m_TargetTransformCom->Get_Pos() + m_CamPreset.pivotOffset;

    // 감속 보간
    if (timeDelta > 0.f)
    {
        m_CurrentPivotPos = Engine::Lerp_Damp(m_CurrentPivotPos,
                                              targetPos,
                                              m_CamPreset.lagSpeed,
                                              timeDelta);

        m_CurrentDistance = Engine::Lerp_Damp(m_CurrentDistance,
                                              m_CamPreset.dist,
                                              m_CamPreset.zoomSpeed,
                                              timeDelta);
    }
    else // timeDelta <= 0.f 경우 현재 위치를 목표 위치로 즉시 설정
    {
        m_CurrentPivotPos = targetPos;
        m_CurrentDistance = m_CamPreset.dist;
    }

    // 카메라 흔들림 업데이트
    Update_Shake(timeDelta);

    // 회전 행렬 생성
    _matrix matRot;
    D3DXMatrixRotationYawPitchRoll(&matRot, m_CamPreset.yaw, m_CamPreset.pitch, 0.f);

    // 최종 위치 계산

    // Local
    _vec3 offsetDist = { 0.f, 0.f, -m_CurrentDistance };

    // Local + Shake
    _vec3 localPos = offsetDist + m_ShakeOffset;

    // (Local + Shake) * Rotation
    _vec3 worldOffset;
    D3DXVec3TransformCoord(&worldOffset, &localPos, &matRot);

    // (Local + Shake) * Rotation + Pivot
    _vec3 finalPos = worldOffset + m_CurrentPivotPos;

    m_TransformCom->Set_Pos(finalPos);

    if (m_CamPreset.dist > 0.01f)
        m_TransformCom->LookAt(m_CurrentPivotPos);
    else
        m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));

    return true;
}

void CCameraMovement::Clamp_Pitch()
{
    constexpr _float rotXLimit = D3DXToRadian(89.f);

    m_CamPreset.pitch = Engine::Clamp(m_CamPreset.pitch, -rotXLimit, rotXLimit);
}

void CCameraMovement::Update_Shake(const _float& timeDelta)
{
    m_ShakeTimeAcc += timeDelta;

    if (m_ShakeAmp > 0.f)
    {
        // 원형 회전 진동 (X, Y축)
        _float t = m_ShakeTimeAcc * m_ShakeFreq;

        // Z축은 흔들지 않음 (깊이는 유지)
        _vec3 localShake(sinf(t), cosf(t), 0.f);

        m_ShakeOffset = localShake * m_ShakeAmp;

        // 선형 감쇠
        m_ShakeAmp -= m_ShakeDecay * timeDelta;

        if (m_ShakeAmp < 0.f)
        {
            m_ShakeAmp    = 0.f;
            m_ShakeOffset = _vec3(0.f, 0.f, 0.f);
        }
    }
    else
        m_ShakeOffset = _vec3(0.f, 0.f, 0.f);
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
