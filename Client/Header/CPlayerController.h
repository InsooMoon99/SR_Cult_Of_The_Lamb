#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCombatStat;
class CState;
class CRigidbody;
class CCollider;
END

/// @brief 플레이어의 조작을 담당하는 컴포넌트
class CPlayerController : public CComponent
{
private:
    struct KeyInputData
    {
        _vec3 moveDir      = { 0.f, 0.f, 0.f };
        _int  axisX        = 0;
        _int  axisZ        = 0;
        _bool hasMoveInput = false;
    };

    struct RollData
    {
        _float rollElapsedTime = 0.f;
        _float rollDuration    = 0.4f;    // 구르기 지속시간
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
    _float Get_RollDuration() const { return m_RollData.rollDuration; }

public: // Setter
    void Set_RollDuration(_float duration) { m_RollData.rollDuration = duration; }

public:
    // m_OwnerStateCom 등 Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수
    HRESULT Init_OwnerComponent();

private:
    void Update_InputData();    // 입력 처리

    void Update_State();        // 상태 전환

    void Update_Dir();          // 현재 방향(Enum)에 맞춰 이동 벡터를 계산하는 함수

    void ChangeDirState();      // 입력에 따라 방향(Enum)만 바꾸는 함수

    void State_Roll(const _float& timeDelta);

    _bool CanApplyDir(ACTORSTATE currentState); // 상태에 따라 방향 변경 가능 여부 판단

    void  Determine_State(); // 입력을 바탕으로 State에 상태 변경 요청

    _bool CanApplyMovement(ACTORSTATE currentState); // 상태에 따라 이동 가능 여부 판단

    void  Apply_Movement(const _float& timeDelta); // 입력을 바탕으로 이동

private:
#pragma region Caching owner components
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    Engine::CRigidbody*  m_OwnerRigidbodyCom;
    Engine::CCollider*   m_OwnerColliderCom;
#pragma endregion

    KeyInputData m_InputData;
    RollData     m_RollData;

public:
    static CPlayerController* Create(DEVICE graphicDev);

    CComponent* Clone() override;

private:
    void Free() override;
};
