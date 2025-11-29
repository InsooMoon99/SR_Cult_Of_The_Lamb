#include "pch.h"
#include "CPlayerAttack.h"

#include "CAttackBox.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CGameObject.h"
#include "CPlayerInput.h"
#include "CState.h"
#include "CTransform.h"
#include "CWeaponEquip.h"

CPlayerAttack::CPlayerAttack(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerWeaponCom(nullptr),
      m_OwnerInputCom(nullptr),

      m_CurrentHitbox(nullptr),

      m_AttackInfo{},
      m_ComboIndex(0),

      m_Timer(0.f),
      m_IsHitboxActive(false),
      m_CanNextCombo(false),
      m_IsNextComboReserved(false)
{ }

CPlayerAttack::CPlayerAttack(const CPlayerAttack& rhs)
    : CComponent(rhs),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerWeaponCom(nullptr),
      m_OwnerInputCom(nullptr),

      m_CurrentHitbox(nullptr),

      m_ComboIndex(rhs.m_ComboIndex),

      m_Timer(rhs.m_Timer),
      m_IsHitboxActive(rhs.m_IsHitboxActive),
      m_CanNextCombo(rhs.m_CanNextCombo),
      m_IsNextComboReserved(rhs.m_IsNextComboReserved)
{
    copy_n(&rhs.m_AttackInfo[0][0],
           static_cast<_int>(WEAPON_MAX) * static_cast<_int>(ATTACK_MAX),
           &m_AttackInfo[0][0]);
}

CPlayerAttack::~CPlayerAttack()
{ }

HRESULT CPlayerAttack::Ready_PlayerCombat()
{
    // =========================================================
    // [1] 검 (WEAPON_SWORD) 데이터 설정
    // =========================================================

    // 1타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].state            = ACTORSTATE::PLAYER_SWORD_COMBO1;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].duration         = 0.5f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxStartTime  = 0.15f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxEndTime    = 0.35f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].comboInputTime   = 0.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].spawnOffset      = 5.f; // 가까움
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxScale      = _vec3(1.f, 1.f, 1.f);

    // 2타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].state            = ACTORSTATE::PLAYER_SWORD_COMBO2;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].duration         = 0.5f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxStartTime  = 0.15f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxEndTime    = 0.35f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].comboInputTime   = 0.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].spawnOffset      = 5.0f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxScale      = _vec3(1.f, 1.f, 1.f);

    // 3타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].state            = ACTORSTATE::PLAYER_SWORD_COMBO3;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].duration         = 0.8f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].hitboxStartTime  = 0.2f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].hitboxEndTime    = 0.5f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].comboInputTime   = 0.5f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].spawnOffset      = 6.0f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxScale      = _vec3(1.2f, 1.f, 1.2f);

    //TODO 강공격 (HEAVY)
    //m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].state           = ACTORSTATE::PLAYER_SWORD_HEAVY; // 가정
    //m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].duration        = 1.5f;
    //m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].hitboxStartTime = 0.5f;
    //m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].hitboxEndTime   = 1.2f;
    //m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].comboInputTime  = 0.0f; // 강공격은 연계 없음

    // =========================================================
    //TODO [2] 건틀릿 (GAUNTLET) 데이터 설정
    // =========================================================

    return S_OK;
}

_int CPlayerAttack::Update_Component(const _float& timeDelta)
{
    // 매 프레임 작업 없음. 스태틱 컴포넌트로 사용할 것
    return 0;
}

void CPlayerAttack::LateUpdate_Component()
{ }

HRESULT CPlayerAttack::Init_OwnerComponent()
{
    m_OwnerStateCom = dynamic_cast<CState*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerWeaponCom = dynamic_cast<CWeaponEquip*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP));

    m_OwnerInputCom = dynamic_cast<CPlayerInput*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_INPUT));

    HRESULT result = S_OK;

    if (nullptr == m_OwnerStateCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner State Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerCombatStatCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner CombatStat Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerWeaponCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner WeaponEquip Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerInputCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner PlayerInput Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    return result;
}

void CPlayerAttack::Start_NormalAttack()
{
    if (!m_OwnerWeaponCom || !m_OwnerWeaponCom->Get_WeaponEquipped()) return;

    Reset_AttackData();

    // 무기가 어떤건지 인덱스 확인
    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return;

    // 1타(COMBO_1) 시작
    m_ComboIndex = static_cast<_int>(ATTACK_COMBO1);
    m_OwnerStateCom->Change_State(m_AttackInfo[weaponIdx][ATTACK_COMBO1].state);
}

void CPlayerAttack::Try_NextCombo()
{
    if (m_CanNextCombo)
        m_IsNextComboReserved = true;
}

_bool CPlayerAttack::Update_NormalAttack(const _float& timeDelta)
{
    if (!m_OwnerWeaponCom || !m_OwnerCombatStatCom) return false;

    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return false;

    // 콤보 인덱스 유효성 체크
    if (m_ComboIndex < ATTACK_COMBO1 || m_ComboIndex > ATTACK_COMBO3) return false;

    m_Timer += timeDelta;

    const AttackInfo& info = m_AttackInfo[weaponIdx][m_ComboIndex];

    _float atkSpeed = m_OwnerCombatStatCom->Get_AttackSpeed();
    if (atkSpeed < 0.1f) atkSpeed = 0.1f;

    // 히트박스 제어
    if (m_Timer >= info.hitboxStartTime / atkSpeed &&
        m_Timer <= info.hitboxEndTime / atkSpeed)
    {
        if (!m_IsHitboxActive)
        {
            m_IsHitboxActive = true;

            //!! 검 기본공격의 경우 좌우만 판단해서 히트박스를 만들면 되기에 mouseDir의 x 성분만 가져와 사용
            _vec3 mouseDir = m_OwnerInputCom->Get_MouseDir();
            mouseDir.z     = 0.f;
            D3DXVec3Normalize(&mouseDir, &mouseDir);

            Create_Hitbox(info, mouseDir);
        }
    }
    else
    {
        if (m_IsHitboxActive)
            m_IsHitboxActive = false;
    }

    // 선입력 허용
    if (m_Timer >= info.comboInputTime / atkSpeed && m_ComboIndex < ATTACK_COMBO3)
        m_CanNextCombo = true;

    // 종료 or 콤보
    if (m_Timer >= info.duration / atkSpeed)
    {
        if (m_IsNextComboReserved && m_ComboIndex < ATTACK_COMBO3)
        {
            ++m_ComboIndex;
            m_Timer               = 0.f;
            m_IsHitboxActive      = false;
            m_CanNextCombo        = false;
            m_IsNextComboReserved = false;

            m_OwnerStateCom->Change_State(m_AttackInfo[weaponIdx][m_ComboIndex].state);
            return true;
        }

        Reset_AttackData();
        return false;
    }

    return true;
}

void CPlayerAttack::Start_HeavyAttack()
{
    if (!m_OwnerWeaponCom || !m_OwnerWeaponCom->Get_WeaponEquipped()) return;

    Reset_AttackData();

    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return;

    m_ComboIndex = ATTACK_HEAVY;
    m_OwnerStateCom->Change_State(m_AttackInfo[weaponIdx][ATTACK_HEAVY].state);
}

_bool CPlayerAttack::Update_HeavyAttack(const _float& timeDelta)
{
    if (!m_OwnerWeaponCom || !m_OwnerCombatStatCom) return false;

    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return false;

    m_Timer += timeDelta;

    const AttackInfo& info = m_AttackInfo[weaponIdx][ATTACK_HEAVY];

    _float atkSpeed = m_OwnerCombatStatCom->Get_AttackSpeed();
    if (atkSpeed < 0.1f) atkSpeed = 0.1f;

    // 히트박스 제어
    if (m_Timer >= info.hitboxStartTime / atkSpeed &&
        m_Timer <= info.hitboxEndTime / atkSpeed)
    {
        if (!m_IsHitboxActive)
        {
            m_IsHitboxActive = true;
            Create_Hitbox(info, m_OwnerInputCom->Get_MouseDir());
        }
    }
    else
    {
        if (m_IsHitboxActive)
            m_IsHitboxActive = false;
    }

    // 종료 체크
    if (m_Timer >= info.duration / atkSpeed)
    {
        Reset_AttackData();
        return false;
    }

    return true;
}

void CPlayerAttack::Cancel_Attack()
{
    Reset_AttackData();
}

_int CPlayerAttack::Get_WeaponIndex(_int weaponID)
{
    switch (weaponID)
    {
    case 3001: return WEAPON_SWORD;
    case 3002: return WEAPON_GAUNTLET;
    default: return -1;
    }
}

void CPlayerAttack::Reset_AttackData()
{
    m_Timer = 0.f;

    if (m_IsHitboxActive)
    {
        m_IsHitboxActive = false;

        // 히트박스 수명이 아직 남아있으면, 강제로 히트박스를 소멸
        // 소멸 시, Create_Hitbox() 에서 등록해놓은 콜백 함수가 자동으로 실행되어
        // m_CurrentHitbox = nullptr로 안전하게 초기화함
        if (m_CurrentHitbox)
        {
            m_CurrentHitbox->Mark_As_PendingKill();
            m_CurrentHitbox = nullptr;
        }
    }

    m_CanNextCombo        = false;
    m_IsNextComboReserved = false;
}

void CPlayerAttack::Create_Hitbox(const AttackInfo& attackInfo, const _vec3& vAttackDir)
{
    if (!m_OwnerCombatStatCom) return;

    // 히트박스 생성
    auto attackBox = CAttackBox::Create(m_GraphicDev);
    NULL_CHECK(attackBox);

    attackBox->Set_Instigator(m_Owner);

    // 히트박스 데이터 세팅
    HITDATA hitData{};
    hitData.damage    = m_OwnerCombatStatCom->Get_Attack() * attackInfo.damageMultiplier;
    hitData.knockback = attackInfo.knockbackPower;
    hitData.duration  = attackInfo.hitboxEndTime - attackInfo.hitboxStartTime;
    attackBox->Set_HitDesc(hitData);

    // 위치 및 크기 설정
    _vec3 ownerPos = m_TransformCom->Get_Pos();

    auto hitboxTransform = dynamic_cast<CTransform*>(
        attackBox->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    NULL_CHECK(hitboxTransform);

    _vec3 spawnPos = ownerPos + (vAttackDir * attackInfo.spawnOffset);

    hitboxTransform->Set_Pos(spawnPos);

    hitboxTransform->Set_Scale(attackInfo.hitboxScale);

    // 현재 씬의 레이어에 히트박스 오브젝트를 추가
    SpawnAttackObject(attackBox);

    m_CurrentHitbox = attackBox;

    // 현재 attackBox가 죽을 때, 이쪽의 m_CurrentHitbox가 아직 현재 attackBox를 가리키고 있으면
    // 이를 nullptr로 안전하게 초기화하는 람다함수를 등록
    m_CurrentHitbox->Set_OnDestroyCallback([this, attackBox]()
    {
        if (m_CurrentHitbox == attackBox)
            m_CurrentHitbox = nullptr;
    });
}

CPlayerAttack* CPlayerAttack::Create(DEVICE graphicDev)
{
    auto playerAttack = new CPlayerAttack(graphicDev);

    if (FAILED(playerAttack->Ready_PlayerCombat()))
    {
        Safe_Release(playerAttack);
        MSG_BOX("PlayerCombat Create Failed");
        return nullptr;
    }

    return playerAttack;
}

CComponent* CPlayerAttack::Clone()
{
    return new CPlayerAttack(*this);
}

void CPlayerAttack::Free()
{
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerWeaponCom     = nullptr;
    m_OwnerInputCom      = nullptr;

    // CPlayerAttack이 현재 히트박스보다 먼저 죽는 경우
    // Set_OnDestroyCallback이 아무것도 실행하지 말도록 nullptr로 콜백할 함수를 지움
    if (m_CurrentHitbox)
    {
        m_CurrentHitbox->Set_OnDestroyCallback(nullptr);
        m_CurrentHitbox = nullptr;
    }

    CComponent::Free();
}
