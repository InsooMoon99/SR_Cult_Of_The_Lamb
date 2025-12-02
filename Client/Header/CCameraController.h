#pragma once
#include "CComponent.h"

class CCameraMovement;
BEGIN(Engine)
class CCameraCalc;
class CTransform;
END

class CCameraController : public CComponent
{
private:
    explicit CCameraController(DEVICE graphicDev);
    explicit CCameraController(const CCameraController& rhs);
    ~CCameraController() override;

public:
    /// @brief 프로토타입 생성 과정에서 초기 Create 호출 시 한번만 실행되는 초기화 함수
    /// @note 리소스 및 프로토타입 로딩 단계에서 실행되기에, 내부 값 초기화로만 쓸 수 있다
    HRESULT Ready_CameraController();

    /// @brief 각 씬의 Initialize_Scene에서 초기 호출하는 초기화 함수
    /// @note 외부 컴포넌트 및 데이터 캐싱 등에 사용할 수 있다
    void Initialize_Component() override;

    _int Update_Component(const _float& timeDelta) override;
    void LateUpdate_Component() override;

public:  // --- Getter ---
    CAM_MODE  Get_CamMode() const { return m_CamMode; }
    VIEW_TYPE Get_ViewType() const { return m_ViewType; }
    PROJ_TYPE Get_ProjType() const { return m_ProjType; }

public:  // --- Getters for Inspector --- : 필요할 경우에 사용

public:  // --- Setter ---
    // 카메라 오프셋 (타겟과의 거리) 설정
    void Set_Offset(const _vec3& offset) { m_Offset = offset; }

    /// @brief 타겟을 중심으로 하는 구면 좌표계 방식으로 오프셋을 설정하는 기능
    /// @details 거리와 X/Y축 회전 각도를 받아, 타겟 기준의 상대적 위치 벡터를 계산
    /// @param distance  타겟과 카메라 사이의 거리
    /// @param angleX    X축 회전 각도(Pitch: 상하 회전), Degree 값
    /// @param angleY    Y축 회전 각도(Yaw: 상하 회전), Degree 값
    void Set_Offset_Spherical(_float distance, _float angleX, _float angleY);

    // 카메라 컨트롤 모드 변경
    void Set_CamMode(CAM_MODE camMode);

    // 시점 타입 변경
    void Set_ViewType(VIEW_TYPE viewType);

    //TODO : 이건 지워야 할듯?
    void Set_ProjType(PROJ_TYPE projType);

    //TODO : 타겟은 CCameraMovement로 옮겼음. 아마 지워야 할듯
    void Set_CamTarget(Engine::CTransform* targetTransform);

public:  // --- 기능 메서드 ---

private: // --- 내부 로직 ---
    void Control_FreeMode(const _float& timeDelta);

private: // --- Owner 컴포넌트 캐싱 ---
    Engine::CCameraCalc* m_OwnerCamCalcCom;
    CCameraMovement*     m_OwnerCamMovementCom;

private: // --- 외부 컴포넌트 캐싱 ---
    Engine::CTransform* m_TargetTransformCom;

private: // --- 일반 멤버 변수 ---
    CAM_MODE  m_CamMode;
    VIEW_TYPE m_ViewType;
    PROJ_TYPE m_ProjType;

    _vec3 m_Offset;        // 3인칭 카메라의 타겟 기준 오프셋 (거리)

public:
    static CCameraController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;
};
