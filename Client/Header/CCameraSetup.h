#pragma once
#include "CCameraMovement.h"

BEGIN(CameraPresets) // 미리 설정된 프리셋 값
inline constexpr CAM_PRESET Default =
    { _vec3(0, 0, 0), 10.f, D3DXToRadian(45.f), 0.f, 5.f, 5.f };

inline constexpr CAM_PRESET Combat =
    { _vec3(0, 1, 0), 6.f, D3DXToRadian(45.f), 0.f, 10.f, 5.f };

inline constexpr CAM_PRESET Boss =
    { _vec3(0, 5, 0), 15.f, D3DXToRadian(45.f), 0.f, 3.f, 5.f };
END

// Builder 패턴을 이용한 CCameraMovement의 값 설정 클래스
class CCameraSetup
{
public:
    /// @brief 변경할 CCameraMovement 컴포넌트를 받아 값 수정
    /// @param camMovementCom  CCameraMovement 컴포넌트 주소
    /// @param LoadCurrent     true면 현재 카메라 설정을 복사해 거기서 추가 수정
    CCameraSetup(CCameraMovement* camMovementCom, _bool LoadCurrent = true)
        : m_CamMovementCom(camMovementCom),
          m_Snap(false)
    {
        if (LoadCurrent && m_CamMovementCom)
            m_CamPreset = m_CamMovementCom->Get_Preset();
        else
            m_CamPreset = CameraPresets::Default;
    }

    // --- 체이닝 메서드 : CameraPresets 이용 ---

    // 프리셋 통째로 로드
    // [현재 사용 안함]
    //CCameraSetup& LoadPreset(const CAM_PRESET& preset)
    //{
    //    m_CamPreset = preset;
    //    return *this;
    //}

    // --- 체이닝 메서드 : 개별 제어 ---

    // 타겟의 어느 지점을 바라볼건지에 대한 로컬 오프셋 설정
    CCameraSetup& PivotOffset(const _vec3& offset)
    {
        m_CamPreset.pivotOffset = offset;
        return *this;
    }

    // 타겟과의 거리 설정
    CCameraSetup& Distance(_float dist)
    {
        m_CamPreset.dist = dist;
        return *this;
    }

    // X축 회전 (Pitch) 설정 [단위: Degree]
    CCameraSetup& RotX(_float angleDegree)
    {
        m_CamPreset.pitch = D3DXToRadian(angleDegree);
        return *this;
    }

    // Y축 회전 (Yaw) 설정 [단위: Degree]
    CCameraSetup& RotY(_float angleDegree)
    {
        m_CamPreset.yaw = D3DXToRadian(angleDegree);
        return *this;
    }

    // 위치 추적 속도 (값이 클수록 빠르게 따라감)
    CCameraSetup& LagSpeed(_float speed)
    {
        m_CamPreset.lagSpeed = speed;
        return *this;
    }

    // 줌 변경 속도 (값이 클수록 빠르게 변경됨)
    CCameraSetup& ZoomSpeed(_float speed)
    {
        m_CamPreset.zoomSpeed = speed;
        return *this;
    }

    // 적용 시 보간 없이 즉시 이동
    CCameraSetup& Snap()
    {
        m_Snap = true;
        return *this;
    }

    // --- 최종 적용 ---

    // 설정한 값을 컴포넌트에 반영 (※ 반드시 마지막에 호출할 것)
    void Apply()
    {
        if (m_CamMovementCom)
            m_CamMovementCom->Apply_Preset(m_CamPreset, m_Snap);
    }

private:
    CCameraMovement* m_CamMovementCom;
    CAM_PRESET       m_CamPreset;
    _bool            m_Snap;
};
