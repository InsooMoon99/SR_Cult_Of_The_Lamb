#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCombatStat;
class CState;
class CRigidbody;
END

/// @brief 플레이어의 조작을 담당하는 컴포넌트
class CPlayerController : public CComponent
{
private:
    struct KeyInputData
    {
        _vec3 moveDir = { 0.f, 0.f, 0.f };
        _int  axisX{};
        _int  axisZ{};
        _bool isMove{};
    };

    struct RollData
    {
        _vec3  rollDir = { 0.f, 0.f, 0.f };
        _bool  isRolling{};
        _float rollElapsedTime{};
        _float rollDuration{ 0.4f };
    };

private:
    explicit CPlayerController(DEVICE graphicDev);
    explicit CPlayerController(const CPlayerController& rhs);
    ~CPlayerController() override;

public:
    HRESULT Ready_PlayerController();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public: // Getter
    _bool  Get_IsRolling() const { return m_RollData.isRolling; }
    _float Get_RollDuration() const { return m_RollData.rollDuration; }

public: // Setter
    void Set_IsRolling(_bool isRolling) { m_RollData.isRolling = isRolling; }
    void Set_RollDuration(_float duration) { m_RollData.rollDuration = duration; }

public:
    // m_OwnerStateCom 등 Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수
    HRESULT Init_OwnerComponent();

private:
    void Key_Input(const _float& timeDelta);
    void Update_Roll(const _float& timeDelta);

    void  Calc_InputVector(KeyInputData& inputData);    // 입력 벡터 및 축 값 계산
    _bool CanApplyDirection(ACTORSTATE currentState);   // 상태에 따라 방향 변경 가능 여부 판단
    void  Apply_Direction(KeyInputData& inputData);     // 입력을 바탕으로 애니메이션 방향 설정
    void  Determine_State(KeyInputData& inputData);     // 입력을 바탕으로 State에 상태 변경 요청
    _bool CanApplyMovement(ACTORSTATE currentState);    // 상태에 따라 이동 가능 여부 판단
    void  Apply_Movement(KeyInputData& inputData, const _float& timeDelta); // 입력을 바탕으로 이동

private:
#pragma region Caching owner components
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStat;
    Engine::CRigidbody*  m_OwnerRigidbody;
#pragma endregion

    RollData m_RollData;

public:
    static CPlayerController* Create(DEVICE graphicDev);

    CComponent* Clone() override;

private:
    void Free() override;
};
