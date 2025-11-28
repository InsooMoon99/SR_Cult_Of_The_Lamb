#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CState;
class CCombatStat;
class CRigidbody;
END

/// @brief 플레이어의 움직임을 기능을 담당하는 스태틱 컴포넌트
class CPlayerMovement : public CComponent
{
private:
    explicit CPlayerMovement(DEVICE graphicDev);
    explicit CPlayerMovement(const CPlayerMovement& rhs);
    ~CPlayerMovement() override;

public:
    HRESULT Ready_PlayerMovement();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- 초기 세팅 ---
    HRESULT Init_OwnerComponent(); // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수

public:  // --- Getter ---
    const _vec3& Get_RollDir() const { return m_RollDir; }
    _float       Get_RollTimer() const { return m_Timer; }
    _float       Get_RollDuration() const { return m_RollDuration; }
    _float       Get_RollInvincibleTime() const { return m_RollInvincibleTime; }
    _bool        Get_IsInvincibleEnded() const { return m_IsInvincibleEnded; }

public:  // --- Setter ---
    void Set_RollDuration(_float duration) { m_RollDuration = duration; }
    void Set_RollInvincibleTime(_float invincibleTime) { m_RollInvincibleTime = invincibleTime; }
    void Set_IsInvincibleEnded(_bool InvincibleEnded) { m_IsInvincibleEnded = InvincibleEnded; }

public:  // --- 동작 명령 함수 (Controller가 호출) ---
    // RUN
    void Run(const _vec3& moveDir, const _float& timeDelta);

    // IDLE
    void Stop();

    // PLAYER_ROLL 시작
    void Start_Roll(const _vec3& moveDir);

    /// @brief PLAYER_ROLL 매 프레임 업데이트
    /// @return true(구르기 중), false(구르기 종료)
    _bool Update_Roll(const _float& timeDelta);

    //TODO : Impulse(dir, force) 로 공격 등에서 살짝 이동하는거 하기

private: // --- 내부 로직 ---
    void  ChangeDir(const _vec3& dir); // 플레이어 방향 갱신
    _vec3 CalcWorldDir(ACTORDIR dir);  // 플레이어의 ACTORDIR 상태에 따라 8방향 벡터 반환

private: // --- Owner Component 캐싱 ---
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    Engine::CRigidbody*  m_OwnerRigidbodyCom;

private: // --- 멤버 변수 ---
    _vec3 m_RollDir;                // 구르기 방향

    _float m_Timer;                 // 구르기 시간 타이머

    _float m_RollDuration;          // 구르기 지속시간
    _float m_RollInvincibleTime;    // 구르기 중 무적 지속시간

    _bool m_IsInvincibleEnded;      // 무적 끝났는지 여부

public:
    static CPlayerMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;
};
