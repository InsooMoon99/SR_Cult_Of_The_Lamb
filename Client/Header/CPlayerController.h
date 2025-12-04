#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCombatStat;
class CState;
class CRigidbody;
class CCollider;
END

class CPlayerInput;
class CPlayerMovement;
class CPlayerAttack;

/// @brief 플레이어의 전체적인 행동과 상태를 관리하는 중앙 제어 컴포넌트
/// @details 
/// 이 클래스는 플레이어의 두뇌 역할을 하며, 다음과 같은 기능을 수행합니다:
/// - 상태 관리 (State Management): IDLE, RUN, ROLL, ATTACK 등 플레이어의 현재 상태를 판단하고 전환합니다.
/// - 컴포넌트 조율 (Component Coordination): Input, Movement, Attack 등 하위 컴포넌트들을 조율하여 유기적인 동작을 만들어냅니다.
/// - 행동 실행 (Action Execution): 각 상태에 맞는 구체적인 행동(Action_Idle, Action_Run 등)을 매 프레임 실행합니다.
/// @note
/// 주요 협력 컴포넌트:
/// - CPlayerInput: 사용자 입력을 받아옵니다.
/// - CPlayerMovement: 이동 및 물리 연산을 수행합니다.
/// - CPlayerAttack: 공격 및 전투 로직을 처리합니다.
class CPlayerController : public CComponent
{
private:
    // 플레이어 컨트롤러 내 사용하는 타이머용 열거체
    enum PL_TIMER
    {
        TIMER_RUNSOUND,  // 달리기 효과음 재생용

        TIMER_END
    };

private:
    explicit CPlayerController(DEVICE graphicDev);
    explicit CPlayerController(const CPlayerController& rhs);
    ~CPlayerController() override;

public:
    HRESULT Ready_PlayerController();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수
    HRESULT Init_OwnerComponent();

public: // --- Getters for Inspector ---
    CPlayerInput*    Get_InputCom() const { return m_OwnerInputCom; }
    CPlayerMovement* Get_MovementCom() const { return m_OwnerMovementCom; }
    CPlayerAttack*   Get_AttackCom() const { return m_OwnerAttackCom; }

private:
    // --- [핵심 로직 파이프라인] ---
    void CheckStateTransition();                // 상태 전환이 가능한지 판단
    void Update_State(const _float& timeDelta); // 상태에 따른 행동 실행

    // --- [세부 행동 메서드] ---
    void Action_Idle(const _float& timeDelta);      // IDLE 상태
    void Action_Run(const _float& timeDelta);       // RUN 상태
    void Action_Roll(const _float& timeDelta);      // PLAYER_ROLL 상태
    void Action_NormalAttack(const _float& timeDelta);     // PLAYER_SWORD_ ~ 상태

private:
#pragma region Caching owner components
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    Engine::CRigidbody*  m_OwnerRigidbodyCom;
    Engine::CCollider*   m_OwnerColliderCom;

    CPlayerInput*    m_OwnerInputCom;
    CPlayerMovement* m_OwnerMovementCom;
    CPlayerAttack*   m_OwnerAttackCom;
#pragma endregion

    _float m_Timers[TIMER_END]; // enum값을 통해 사용하는 타이머

public:
    static CPlayerController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;
};
