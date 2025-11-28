#include "pch.h"
#include "CPlayerMovement.h"

#include "CCombatStat.h"
#include "CGameObject.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CPlayerMovement::CPlayerMovement(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),

      m_RollDir(0.f, 0.f, 0.f),
      m_RollTimer(0.f),
      m_RollDuration(0.4f),
      m_RollInvincibleTime(0.3f),
      m_IsInvincibleEnded(false)
{ }

CPlayerMovement::CPlayerMovement(const CPlayerMovement& rhs)
    : CComponent(rhs),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),

      m_RollDir(rhs.m_RollDir),
      m_RollTimer(rhs.m_RollTimer),
      m_RollDuration(rhs.m_RollDuration),
      m_RollInvincibleTime(rhs.m_RollInvincibleTime),
      m_IsInvincibleEnded(rhs.m_IsInvincibleEnded)
{ }

CPlayerMovement::~CPlayerMovement()
{ }

HRESULT CPlayerMovement::Ready_PlayerMovement(DEVICE graphicDev)
{
    return S_OK;
}

_int CPlayerMovement::Update_Component(const _float& timeDelta)
{
    //!! Movement는 Controller의 명령(함수 호출)에 의해 수동적으로 동작하므로
    //!! 여기서 능동적으로 업데이트할 내용은 없음
    return 0;
}

void CPlayerMovement::LateUpdate_Component()
{ }

HRESULT CPlayerMovement::Init_OwnerComponent()
{
    m_OwnerStateCom = dynamic_cast<CState*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerRigidbodyCom = dynamic_cast<CRigidbody*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    NULL_CHECK_RETURN_MSG(
        m_OwnerStateCom,
        E_FAIL,
        L"CPlayerMovement: Owner State Null");

    NULL_CHECK_RETURN_MSG(
        m_OwnerCombatStatCom,
        E_FAIL,
        L"CPlayerMovement: Owner CombatStat Null");

    NULL_CHECK_RETURN_MSG(
        m_OwnerRigidbodyCom,
        E_FAIL,
        L"CPlayerMovement: Owner Rigidbody Null");

    return S_OK;
}

void CPlayerMovement::Run(const _vec3& moveDir, const _float& timeDelta)
{ }

void CPlayerMovement::Stop()
{ }

void CPlayerMovement::Start_Roll(const _vec3& moveDir)
{ }

HRESULT CPlayerMovement::Update_Roll(const _float& timeDelta)
{
    m_RollTimer += timeDelta;

    // 1. 구르기 종료 체크
    if (m_RollTimer >= m_RollDuration)
    {
        if (m_OwnerRigidbodyCom)
        {
            m_OwnerRigidbodyCom->Set_UseAccel(true);
            m_OwnerRigidbodyCom->Set_Friction(0.2f);
        }
        return true; // 구르기 끝남
    }

    // 2. 무적 시간 종료 체크
    if (!m_IsInvincibleEnded && m_RollTimer >= m_RollInvincibleTime)
    {
        m_IsInvincibleEnded = true;
        // TODO: 무적 상태 해제 로직
    }

    // 3. 구르기 이동 (좌표 직접 이동)
    _float rollSpeed = m_OwnerCombatStatCom->Get_Speed() * 3.f;
    m_TransformCom->Move_Pos(m_RollDir, timeDelta, rollSpeed);

    return S_OK;
}

_bool CPlayerMovement::CanChangeDir()
{
    return false;
}

void CPlayerMovement::ChangeDir(const _vec3& dir)
{ }

_vec3 CPlayerMovement::CalcWorldDir(ACTORDIR dir)
{
    return {};
}

CPlayerMovement* CPlayerMovement::Create(DEVICE graphicDev)
{
    return nullptr;
}

CComponent* CPlayerMovement::Clone()
{
    return nullptr;
}

void CPlayerMovement::Free()
{
    CComponent::Free();
}
