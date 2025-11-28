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
    // 플레이어 상태 머신 제어용 데이터
    struct PlayerStateData
    {
        // ==========================================
        // [1] 공통 타이머
        // ==========================================
        _float timer = 0.f;

        // ==========================================
        // [2] 방향 데이터
        // ==========================================
        _vec3 moveDir   = { 0.f, 0.f, 0.f }; // WASD 이동 방향 (이동용)
        _vec3 rollDir   = { 0.f, 0.f, 0.f }; // 구르기 시작시 저장하는 방향
        _vec3 attackDir = { 0.f, 0.f, 0.f }; // 마우스/타겟 방향 (공격용)

        // ==========================================
        // [3] 구르기
        // ==========================================
        _bool isRollInvincibleEnded = false; // 구르기 무적시간 끝났는지 여부 

        _float rollDuration           = 0.4f; // 구르기 전체 시간
        _float rollInvincibleDuration = 0.3f; // 구르기 무적 시간 (초반 0.3초만 무적)

        // ==========================================
        // [4] 일반공격 공통
        // ==========================================
        _bool isHitboxActive      = false; // 공격 판정 박스가 현재 켜져 있는지 여부
        _bool isHeavyAttack       = false; // 현재 공격이 강공격인지 여부
        _bool canNextCombo        = false; // "지금 클릭하면 다음 콤보 예약됨?" (선입력 허용 구간 플래그)
        _bool isNextComboReserved = false; // "다음 콤보가 예약되었음" (애니메이션 종료 시 다음 상태로 전이)

        _int comboIndex = 0; // 현재 콤보 단계 (0:없음, 1:1타, 2:2타, 3:3타)

        // ==========================================
        // [5] 검 공격
        // ==========================================
        _float swordCombo1Duration      = 0.5f; // 1타 길이
        _float swordCombo2Duration      = 0.5f; // 2타 길이
        _float swordCombo3Duration      = 0.5f; // 3타 길이
        _float swordHeavyAttackDuration = 1.0f; // 강공격 길이

        // 현재 검 공격의 지속시간 Getter
        _float Get_SwordAttackDuration(_float attackSpeed)
        {
            _float duration = 0.8f;

            if (isHeavyAttack)
                duration = swordHeavyAttackDuration;
            else
            {
                switch (comboIndex)
                {
                case 1: duration = swordCombo1Duration;
                    break;
                case 2: duration = swordCombo2Duration;
                    break;
                case 3: duration = swordCombo3Duration;
                    break;
                }
            }

            if (attackSpeed > 0.f)
                return duration / attackSpeed;

            return duration;
        }

        // TODO : FSM 관련 필요한 데이터 추가하기

        // ==========================================
        // [유틸] 초기화 함수
        // ==========================================
        void Reset()
        {
            //!! 콤보 인덱스는 초기화하지 않음 (콤보 이어가기 로직 때문)
            // 단, IDLE이나 RUN으로 갈 때는 외부에서 0으로 초기화해줘야 함

            timer = 0.f;

            //moveDir   = { 0.f, 0.f, 0.f };
            //rollDir   = { 0.f, 0.f, 0.f };
            //attackDir = { 0.f, 0.f, 0.f };

            isRollInvincibleEnded = false;
            isHitboxActive        = false;
            isHeavyAttack         = false;
            canNextCombo          = false;
            isNextComboReserved   = false;
        }
    };

private:
    explicit CPlayerController(DEVICE graphicDev);
    explicit CPlayerController(const CPlayerController& rhs);
    ~CPlayerController() override;

public:
    HRESULT Ready_PlayerController();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수
    HRESULT Init_OwnerComponent();

public: // Getter
    _float Get_RollDuration() const { return m_StateData.rollDuration; }

public: // Setter
    void Set_RollDuration(_float duration) { m_StateData.rollDuration = duration; }

private:
    // --- [핵심 로직 파이프라인] ---
    void Update_Input();                        // 입력 처리
    void CheckStateTransition();                // 상태 전환이 가능한지 판단
    void Update_State(const _float& timeDelta); // 상태에 따른 행동 실행

    // --- [방향 제어] ---
    _bool CanChangeDir(ACTORSTATE state);   // 방향 전환 가능 상태인지 확인
    void  ChangeDir();                      // 이동 방향에 맞춰 바라보는 방향 갱신
    _vec3 CalcWorldDir(ACTORDIR dir);       // 플레이어의 ACTORDIR 상태에 따라 월드 기준 방향 반환

    // --- [보조 함수] ---
    _bool IsAttacking(ACTORSTATE state);

    // --- [세부 행동 메서드] ---
    void Action_Idle(const _float& timeDelta);      // IDLE 상태
    void Action_Run(const _float& timeDelta);       // RUN 상태
    void Action_Roll(const _float& timeDelta);      // PLAYER_ROLL 상태
    void Action_Sword(const _float& timeDelta);     // PLAYER_SWORD_ ~ 상태

private:
#pragma region Caching owner components
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    Engine::CRigidbody*  m_OwnerRigidbodyCom;
    Engine::CCollider*   m_OwnerColliderCom;
#pragma endregion

    PlayerStateData m_StateData;

public:
    static CPlayerController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;
};
