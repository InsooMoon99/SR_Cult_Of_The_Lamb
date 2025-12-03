#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCameraCalc;
class CTransform;
END

class CCameraMovement;

class CCameraController : public CComponent
{
private:
    explicit CCameraController(DEVICE graphicDev);
    explicit CCameraController(const CCameraController& rhs);
    ~CCameraController() override;

public:
    HRESULT Ready_CameraController();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    CAM_MODE Get_CamMode() const { return m_CamMode; }

public:  // --- Getter : Imgui 관리용. 타 용도로 사용 금지 ---
    Engine::CCameraCalc* Get_OwnerCamCalc() const { return m_OwnerCamCalcCom; }
    CCameraMovement*     Get_OwnerCamMovement() const { return m_OwnerCamMovementCom; }

public:  // --- Setter ---
    void Set_CamTarget(Engine::CTransform* targetTransform); // 타겟 설정
    void Set_CamMode(CAM_MODE camMode); // 카메라 모드 변경
    void Set_ProjType(PROJ_TYPE projType); // 투영 타입 변경

private: // --- 내부 로직 ---

    // [자유 모드] WASD 이동
    void Control_FreeMode(const _float& timeDelta);
    // [타겟 모드] 타겟 추적 처리 및 예외 상황 핸들링
    void Control_TargetMode(const _float& timeDelta);

private: // --- Owner 컴포넌트 캐싱 ---
    Engine::CCameraCalc* m_OwnerCamCalcCom;
    CCameraMovement*     m_OwnerCamMovementCom;

private: // --- 일반 멤버 변수 ---
    CAM_MODE m_CamMode;

    _float m_FreeMoveSpeed; // 자유 시점 이동 속도
    _float m_FreeRotSpeed;  // 자유 시점 회전 속도 (Degree/sec)

public:
    static CCameraController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;
};
