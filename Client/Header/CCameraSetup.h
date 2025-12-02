#pragma once
#include "CCameraMovement.h"

static const CAM_PRESET PRESET_DEFAULT =
    { _vec3(0, 0, 0), 10.f, D3DXToRadian(45.f), 5.f };
static const CAM_PRESET PRESET_COMBAT =
    { _vec3(0, 1, 0), 6.f, D3DXToRadian(15.f), 10.f };
static const CAM_PRESET PRESET_BOSS =
    { _vec3(0, 5, 0), 15.f, D3DXToRadian(45.f), 3.f };

class CCameraSetup
{
public:
    // 생성자: 변경할 타겟 컴포넌트를 받음
    // LoadCurrent: true면 현재 카메라 설정을 그대로 가져와서 수정 시작
    CCameraSetup(CCameraMovement* camMovementCom, _bool LoadCurrent = true)
        : m_CamMovementCom(camMovementCom),
          m_Immediate(false)
    {
        if (LoadCurrent && m_CamMovementCom)
        {
            // 현재 타겟의 상태를 가져와서 초기값으로 설정 (Get 함수 필요)
            m_Preset.vPivotOffset = m_CamMovementCom->Get_PivotOffset();
            m_Preset.fDist        = m_CamMovementCom->Get_TargetDistance();
            m_Preset.fPitch       = m_CamMovementCom->Get_RotX();
            m_Preset.fYaw         = m_CamMovementCom->Get_RotY();
            // Speed 등은 Getter가 없다면 기본값 사용하거나 추가 필요
        }
    }

    // 특정 프리셋으로 덮어쓰고 시작
    CCameraSetup& LoadPreset(const CAM_PRESET& preset)
    {
        m_Preset = preset;
        return *this;
    }

    // --- 체이닝 메서드들 ---

    CCameraSetup& Distance(_float dist)
    {
        m_Preset.fDist = dist;
        return *this;
    }

    CCameraSetup& RotX(_float angleDegree)
    {
        m_Preset.fPitch = D3DXToRadian(angleDegree);
        return *this;
    }

    CCameraSetup& RotY(_float angleDegree)
    {
        m_Preset.fYaw = D3DXToRadian(angleDegree);
        return *this;
    }

    CCameraSetup& Offset(const _vec3& offset)
    {
        m_Preset.vPivotOffset = offset;
        return *this;
    }

    CCameraSetup& LagSpeed(_float speed)
    {
        m_Preset.fLagSpeed = speed;
        return *this;
    }

    CCameraSetup& ZoomSpeed(_float speed)
    {
        m_Preset.fZoomSpeed = speed;
        return *this;
    }

    // 즉시 이동 여부 (텔레포트)
    CCameraSetup& Immediate()
    {
        m_Immediate = true;
        return *this;
    }

    // --- 최종 적용 ---
    void Apply()
    {
        if (m_CamMovementCom)
            m_CamMovementCom->Apply_Preset(m_Preset, m_Immediate);
    }

private:
    CCameraMovement* m_CamMovementCom;
    CAM_PRESET       m_Preset;     // 임시 저장용 프리셋
    _bool            m_Immediate; // 즉시 적용 여부
};
