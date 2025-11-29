#pragma once
#include "CComponent.h"

class CPlayerInput;
class CWeaponEquip;

BEGIN(Engine)
class CCombatStat;
class CState;
class CAttackBox;
END

/// @brief 플레이어의 무기 공격을 담당하는 스태틱 컴포넌트
class CPlayerAttack : public CComponent
{
public:
    // 무기 공격 정보 구조체
    struct AttackInfo
    {
        ACTORSTATE state;           // 해당 공격의 애니메이션 상태
        _float     duration;        // 공격 지속 시간
        _float     hitboxStartTime; // 히트박스가 켜지는 시간
        _float     hitboxEndTime;   // 히트박스가 꺼지는 시간
        _float     comboInputTime;  // 다음 콤보 선입력 허용 시작 시간

        _float damageMultiplier;    //TODO 공격력 배율. 나중에 아이템 정보와 연동하기
        _float knockbackPower;      // 넉백 힘
        _float spawnOffset;         // 플레이어 전방 생성 거리
        _vec3  hitboxScale;         // 히트박스 크기 (예: 1, 1, 1)
    };

    // [AttackInfo 관리용] 무기 종류 enum
    enum PL_WEAPON_INDEX
    {
        WEAPON_SWORD,      // 무기 : 검
        WEAPON_GAUNTLET,   // 무기 : 건틀릿

        WEAPON_MAX
    };

    // [AttackInfo 관리용] 공격 종류 enum
    enum PL_ATTACK_INDEX
    {
        ATTACK_COMBO1,    // 일반 1타
        ATTACK_COMBO2,    // 일반 2타
        ATTACK_COMBO3,    // 일반 3타
        ATTACK_HEAVY,      // 강공격

        ATTACK_MAX
    };

private:
    explicit CPlayerAttack(DEVICE graphicDev);
    explicit CPlayerAttack(const CPlayerAttack& rhs);
    ~CPlayerAttack() override;

public:
    HRESULT Ready_PlayerCombat();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- 초기 세팅 ---
    HRESULT Init_OwnerComponent(); // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수

public:  // --- Getter ---

public:  // --- 동작 명령 함수 (Controller가 호출) ---

    void  Start_NormalAttack();                         // 일반공격 시작
    void  Try_NextCombo();                              // 일반공격 콤보 예약 (일반공격 중 클릭시 호출)
    _bool Update_NormalAttack(const _float& timeDelta); // 일반공격 중 매 프레임 호출. false면 종료

    void  Start_HeavyAttack();                          // 강공격 시작
    _bool Update_HeavyAttack(const _float& timeDelta);  // 강공격 중 매 프레임 호출. false면 종료

    void Cancel_Attack(); // 공격이 강제 취소될 때 직접 호출

private: // --- 내부 로직 ---
    _int Get_WeaponIndex(_int weaponID);
    void Reset_AttackData();

    /// @brief 공격 정보 구조체와 계산된 방향을 받아 히트박스 생성
    /// @param attackInfo 현재 공격의 설정 데이터 (데미지 배율, 크기, 오프셋 등)
    /// @param vAttackDir 계산된 공격 방향 벡터
    void Create_Hitbox(const AttackInfo& attackInfo, const _vec3& vAttackDir);

private: // --- Owner Component 캐싱 ---
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    CWeaponEquip*        m_OwnerWeaponCom;
    CPlayerInput*        m_OwnerInputCom;

private: // --- 다른 오브젝트 캐싱 ---
    Engine::CAttackBox* m_CurrentHitbox; // 현재 활성화된 히트박스

private: // --- 멤버 변수 ---
    // 무기 공격 데이터 저장소
    AttackInfo m_AttackInfo[WEAPON_MAX][ATTACK_MAX];

    _int   m_ComboIndex;            // 0:없음, 1~3:콤보 인덱스
    _float m_Timer;                 // 공격 시간 타이머

    _bool m_IsHitboxActive;         // 현재 히트박스가 켜져 있는가 여부
    _bool m_CanNextCombo;           // 다음 콤보 선입력 허용 여부
    _bool m_IsNextComboReserved;    // 다음 콤보가 예약되었는지 여부

public:
    static CPlayerAttack* Create(DEVICE graphicDev);
    CComponent*           Clone() override;

private:
    void Free() override;
};
