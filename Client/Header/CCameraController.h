#pragma once
#include "CComponent.h"

class CCameraController : public CComponent
{
public:
    // 카메라 컨트롤 모드
    enum CAM_MODE
    {
        CAM_FREE,               // 자유 카메라
        CAM_TARGET,             // 타겟 고정 카메라

        CAM_END
    };

    // 시점 타입
    enum VIEW_TYPE
    {
        VIEW_FPS,               // 1인칭
        VIEW_TPS,               // 3인칭
        VIEW_QUARTER,           // 쿼터뷰 (2.5D)

        VIEW_END
    };

    // 투영 타입
    enum PROJ_TYPE
    {
        PROJ_PERSPECTIVE,       // 원근 투영
        PROJ_ORTHOGRAPHIC,      // 직교 투영

        PROJ_END
    };

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

public:  // --- Getters for Inspector --- : 필요할 경우에 사용

public:  // --- Setter ---

public:  // --- 기능 메서드 ---

private: // --- 내부 로직 ---

private: // --- Owner Components 캐싱 ---

private: // --- 일반 멤버 변수 ---

public:
    static CCameraController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;
};
