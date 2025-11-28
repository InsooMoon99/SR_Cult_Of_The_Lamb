#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CState;
class CCombatStat;
class CRigidbody;
END

/// @brief 플레이어의 움직임을 담당하는 컴포넌트
class CPlayerMovement : public CComponent
{
private:
    explicit CPlayerMovement(DEVICE graphicDev);
    explicit CPlayerMovement(const CPlayerMovement& rhs);
    ~CPlayerMovement() override;

public:
    HRESULT Ready_PlayerMovement(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:     // --- 초기 세팅 ---
    HRESULT Init_OwnerComponent();  // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수

public:     // --- 동작 명령 함수 (Controller가 호출) ---
    // RUN
    void Run(const _vec3& moveDir, const _float& timeDelta);

    // IDLE
    void Stop();

    // PLAYER_ROLL 진입
    void Start_Roll(const _vec3& moveDir);

    /// @brief PLAYER_ROLL 매 프레임 업데이트
    /// @return 
    HRESULT Update_Roll(const _float& timeDelta);

private:    // --- 내부 로직 ---
    _bool CanChangeDir();              // 방향 전환 가능 여부 체크
    void  ChangeDir(const _vec3& dir); // 플레이어 방향 갱신
    _vec3 CalcWorldDir(ACTORDIR dir);  // 플레이어의 ACTORDIR 상태에 따라 8방향 벡터 반환

private:    // --- Owner Component 캐싱 ---
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    Engine::CRigidbody*  m_OwnerRigidbodyCom;

private:    // --- 멤버 변수 ---
    _vec3  m_RollDir;
    _float m_RollTimer;
    _float m_RollDuration;
    _float m_RollInvincibleTime;
    _bool  m_IsInvincibleEnded;

public:
    static CPlayerMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;
};
