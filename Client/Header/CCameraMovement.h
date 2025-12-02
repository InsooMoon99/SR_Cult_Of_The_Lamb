#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;
END

typedef struct CameraPreset
{
    _vec3  vPivotOffset;    // 타겟 오프셋
    _float fDist;           // 거리
    _float fPitch;          // 상하 각도
    _float fYaw;            // 좌우 각도
    _float fLagSpeed;       // 추적 속도
    _float fZoomSpeed;      // 줌 속도

    // 빠른 초기화
    CameraPreset()
        : vPivotOffset(0.f, 0.f, 0.f),
          fDist(8.f),
          fPitch(D3DXToRadian(45.f)),
          fYaw(0.f),
          fLagSpeed(5.f),
          fZoomSpeed(5.f) {}

    /// @brief 최소한의 조절값으로 초기화
    /// @param offset  오프셋
    /// @param dist    타겟과의 거리
    /// @param rotX    X축 회전
    /// @param lag     추적 속도
    CameraPreset(_vec3 offset, _float dist, _float rotX, _float lag)
        : vPivotOffset(offset),
          fDist(dist),
          fPitch(rotX),
          fYaw(0.f),
          fLagSpeed(lag),
          fZoomSpeed(5.f) {}
} CAM_PRESET;

/// @brief 카메라 움직임을 담당하는 스태틱 컴포넌트
class CCameraMovement : public CComponent
{
private:
    explicit CCameraMovement(DEVICE graphicDev);
    explicit CCameraMovement(const CCameraMovement& rhs);
    ~CCameraMovement() override;

public:
    HRESULT Ready_CameraMovement();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    const _vec3& Get_PivotOffset() const { return m_PivotOffset; }
    _float       Get_TargetDistance() const { return m_TargetDistance; }
    _float       Get_RotX() const { return m_CamPitch; }
    _float       Get_RotY() const { return m_CamYaw; }
    _float       Get_LagSpeed() const { return m_LagSpeed; }
    _float       Get_ZoomSpeed() const { return m_ZoomSpeed; }

public:  // --- Setter ---
    void Set_Target(CTransform* targetTransform);
    void Set_PivotOffset(const _vec3& offset) { m_PivotOffset = offset; }
    void Set_TargetDistance(_float dist) { m_TargetDistance = dist; }
    void Set_RotX(_float rotX);
    void Set_RotY(_float rotY) { m_CamYaw = rotY; }
    void Set_LagSpeed(_float speed) { m_LagSpeed = speed; }
    void Set_ZoomSpeed(_float speed) { m_ZoomSpeed = speed; }

public:  // --- public 함수 ---
    // 통합 초기화 함수 (Reset 용도: 스테이지 진입 등)
    void Reset_Camera(const _vec3& pivotOffset = initOffset,
                      _float       dist        = initDist,
                      _float       pitch       = D3DXToRadian(initPitch),
                      _float       yaw         = D3DXToRadian(initYaw));

    // [자유 모드] 자유 시점 이동
    void Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta);

    /// @brief [자유 모드] 자유 시점 X축 회전
    /// @param angleDelta  초당 회전속도 * timeDelta
    void RotateX_FreeMode(_float angleDelta);

    /// @brief [자유 모드] 자유 시점 Y축 회전
    /// @param angleDelta  초당 회전속도 * timeDelta
    void RotateY_FreeMode(_float angleDelta);

    /// @brief [타겟 모드] 타겟을 따라가는 이동 기능
    /// @details 타겟과의 거리, 타겟으로부터의 각도값을 통해 일정 거리 떨어진 상태로 이동하며,
    /// 감속 보간을 적용해 부드럽게 따라갈 수 있도록 한다.
    /// @return 성공적으로 위치 갱신시 true, 문제 발생시 false -> FreeMode 로 강제 전환
    _bool Update_TargetMode(const _float& timeDelta);

    void CamShake(_float amplitude, _float decay, _float freq = 20.f);

private: // --- private 함수 ---
    void Clamp_Pitch();

private: // --- 외부 컴포넌트 캐싱 ---
    Engine::CTransform* m_TargetTransformCom;

private: // --- 목표 값 ---
    _vec3  m_PivotOffset;       // 타겟의 어디를 바라볼지 (예: (0, 1, 0))
    _float m_TargetDistance;    // 얼마나 멀리 떨어질지 (줌 인/아웃)
    _float m_CamPitch;          // 상하 각도
    _float m_CamYaw;            // 좌우 각도

private: // --- 현재 값 : 보간 계산 중 현재 적용되는 값 ---
    _vec3  m_CurrentPivotPos;   // 현재 바라보고 있는 피벗 위치
    _float m_CurrentDistance;   // 현재 카메라 거리

private: // --- 보간 속도 변수---
    _float m_LagSpeed;          // 타겟 추적 속도
    _float m_ZoomSpeed;         // 줌 변경 속도

private: // --- 카메라 흔들기 변수 ---

public:
    static CCameraMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;
};
