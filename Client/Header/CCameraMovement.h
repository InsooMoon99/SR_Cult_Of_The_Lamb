#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;
END

// 카메라 설정 프리셋
typedef struct CameraPreset
{
    _vec3  pivotOffset;    // 타겟의 어느 지점을 바라볼건지에 대한 오프셋
    _float dist;           // 타겟과의 거리
    _float pitch;          // 상하 각도 (Radian)
    _float yaw;            // 좌우 각도 (Radian)
    _float lagSpeed;       // 추적 속도
    _float zoomSpeed;      // 줌 속도

    // 빠른 초기화
    CameraPreset()
        : pivotOffset(0.f, 0.f, 0.f),
          dist(8.f),
          pitch(D3DXToRadian(45.f)),
          yaw(0.f),
          lagSpeed(5.f),
          zoomSpeed(5.f) {}

    /// @brief 값 지정 초기화
    /// @param _offset     바라보는 지점의 오프셋
    /// @param _dist       타겟과의 거리
    /// @param _rotX       X축 회전
    /// @param _rotY       Y축 회전
    /// @param _lagSpeed   추적 속도
    /// @param _zoomSpeed  줌 속도
    CameraPreset(_vec3  _offset,
                 _float _dist,
                 _float _rotX,
                 _float _rotY,
                 _float _lagSpeed,
                 _float _zoomSpeed)
        : pivotOffset(_offset),
          dist(_dist),
          pitch(_rotX),
          yaw(_rotY),
          lagSpeed(_lagSpeed),
          zoomSpeed(_zoomSpeed) {}
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
    const _vec3& Get_PivotOffset() const { return m_CamPreset.pivotOffset; }
    _float       Get_TargetDistance() const { return m_CamPreset.dist; }
    _float       Get_RotX() const { return m_CamPreset.pitch; }
    _float       Get_RotY() const { return m_CamPreset.yaw; }
    _float       Get_LagSpeed() const { return m_CamPreset.lagSpeed; }
    _float       Get_ZoomSpeed() const { return m_CamPreset.zoomSpeed; }

    const CAM_PRESET& Get_Preset() const { return m_CamPreset; }

public:  // --- Setter ---
    void Set_Target(CTransform* targetTransform);

    void Set_PivotOffset(const _vec3& offset) { m_CamPreset.pivotOffset = offset; }
    void Set_TargetDistance(_float dist) { m_CamPreset.dist = dist; }
    void Set_RotX(_float rotX);
    void Set_RotY(_float rotY) { m_CamPreset.yaw = rotY; }
    void Set_LagSpeed(_float speed) { m_CamPreset.lagSpeed = speed; }
    void Set_ZoomSpeed(_float speed) { m_CamPreset.zoomSpeed = speed; }

public:  // --- 공용 기능 ---

    /// @brief CAM_PRESET 구조체로 프리셋 적용
    /// @param preset     카메라 프리셋 구조체
    /// @param snap       true면 보간 없이 즉시 이동
    void Apply_Preset(const CAM_PRESET& preset, _bool snap = false);

    // 통합 초기화 함수 (Reset 용도: 스테이지 진입 등)
    void Reset_Camera(const CAM_PRESET& preset = CAM_PRESET()) { Apply_Preset(preset, true); }

    /// @brief 카메라 흔들기 기능
    /// @param amplitude  진동 크기
    /// @param decay      감쇠 속도
    /// @param freq       진동수
    void Start_CamShake(_float amplitude, _float decay, _float freq = 20.f);

public:  // --- 자유 모드 기능 ---
    void Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta);
    void RotateX_FreeMode(_float angleDelta);
    void RotateY_FreeMode(_float angleDelta);

public:  // --- 타겟 모드 기능 ---

    /// @brief [타겟 모드] 타겟을 따라가는 이동 기능
    /// @details 타겟과의 거리, 타겟으로부터의 각도값을 통해 일정 거리 떨어진 상태로 이동하며,
    /// 감속 보간을 적용해 부드럽게 따라갈 수 있도록 한다.
    /// @return 성공적으로 위치 갱신시 true, 문제 발생시 false -> FreeMode 로 강제 전환
    _bool Update_TargetMode(const _float& timeDelta);

private: // --- 내부 기능 ---
    void Clamp_Pitch();
    void Update_Shake(const _float& timeDelta);

private: // --- 외부 컴포넌트 캐싱 ---
    Engine::CTransform* m_TargetTransformCom;

private: // --- 멤버 변수 ---
    CAM_PRESET m_CamPreset;

private: // --- 현재 값 : 보간 계산 중 현재 적용되는 값 ---
    _vec3  m_CurrentPivotPos;   // 현재 바라보고 있는 피벗 위치
    _float m_CurrentDistance;   // 현재 카메라 거리

private: // --- 카메라 흔들기 변수 ---
    _float m_ShakeAmp;      // 현재 진폭
    _float m_ShakeDecay;    // 감쇠 속도
    _float m_ShakeFreq;     // 주파수
    _float m_ShakeTimeAcc;  // 시간 누적
    _vec3  m_ShakeOffset;   // 계산된 오프셋

public:
    static CCameraMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;
};
