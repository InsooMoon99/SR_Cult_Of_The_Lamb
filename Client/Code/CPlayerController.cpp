#include "pch.h"
#include "CPlayerController.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CMainEditorMgr.h"
#include "CPlayer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CPlayerController::CPlayerController(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),
      m_OwnerColliderCom(nullptr)
{ }

CPlayerController::CPlayerController(const CPlayerController& rhs)
    : CComponent(rhs),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),
      m_OwnerColliderCom(nullptr)
{ }

CPlayerController::~CPlayerController()
{ }

HRESULT CPlayerController::Ready_PlayerController()
{
    return S_OK;
}

_int CPlayerController::Update_Component(const _float& timeDelta)
{
    // 일시정지 -> 입력 무시
    if (!CMainEditorMgr::GetInstance()->IsPlaying())
        return 0;

    // 1. 입력 수집
    Update_Input();

    // 2. 상태 전환 판단
    CheckStateTransition();

    // 3. 현재 상태에 따른 액션 수행
    Update_State(timeDelta);

    return 0;
}

void CPlayerController::LateUpdate_Component()
{ }

HRESULT CPlayerController::Init_OwnerComponent()
{
    m_OwnerStateCom = dynamic_cast<CState*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerRigidbodyCom = dynamic_cast<CRigidbody*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    m_OwnerColliderCom = dynamic_cast<CBoxCollider*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::BOX_COLL));

    NULL_CHECK_RETURN_MSG(
        m_OwnerStateCom,
        E_FAIL,
        L"CPlayerController: Owner's STATE caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerCombatStatCom,
        E_FAIL,
        L"CPlayerController: Owner's COMBATSTAT caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerRigidbodyCom,
        E_FAIL,
        L"CPlayerController: Owner's RIGIDBODY caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerColliderCom,
        E_FAIL,
        L"CPlayerController: Owner's BOXCOLLIDER caching has failed");

    return S_OK;
}

void CPlayerController::Update_Input()
{
    const auto inputMgr = CDInputMgr::GetInstance();

    _int axisX = 0;
    _int axisZ = 0;

    // 키 입력 체크
    if (inputMgr->KeyPress(DIK_W)) axisZ++;
    if (inputMgr->KeyPress(DIK_S)) axisZ--;
    if (inputMgr->KeyPress(DIK_D)) axisX++;
    if (inputMgr->KeyPress(DIK_A)) axisX--;

    // MoveDir 벡터 생성
    m_StateData.moveDir = _vec3(static_cast<_float>(axisX),
                                0.f,
                                static_cast<_float>(axisZ));

    // 벡터 정규화 및 플래그 설정
    if (D3DXVec3LengthSq(&m_StateData.moveDir) > 0.001f)
        D3DXVec3Normalize(&m_StateData.moveDir, &m_StateData.moveDir);
}

void CPlayerController::CheckStateTransition()
{
    ACTORSTATE curState = m_OwnerStateCom->Get_State();
    const auto inputMgr = CDInputMgr::GetInstance();

    //!! 상태 변경 불가 조건 (피격, 사망)
    if (curState == ACTORSTATE::DEAD || curState == ACTORSTATE::HIT)
        return;

    // [우선순위 1] 구르기 (Space)
    // - 공격 상태여도 구르기로 이행 (캔슬)
    if (inputMgr->KeyDown(DIK_SPACE))
    {
        if (curState != ACTORSTATE::PLAYER_ROLL)
        {
            // 구르기 진입 세팅
            m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_ROLL);

            m_StateData.Reset();

            // 구르기 시작 시 방향 결정
            if (D3DXVec3LengthSq(&m_StateData.moveDir) > 0.f)
                m_StateData.rollDir = m_StateData.moveDir;
            else
                m_StateData.rollDir = CalcWorldDir(m_OwnerStateCom->Get_Dir());

            // TODO : 문제점 추후 해결
            /*
            현재 구르기(ACTORSTATE::PLAYER_ROLL) 진입 시 m_OwnerColliderCom->Set_CollisionActive(false)를 호출하고 있습니다.
    
            문제점:
    
            이 코드는 플레이어의 모든 충돌을 꺼버립니다. 즉, 적의 공격뿐만 아니라
            벽이나 장애물도 통과하게 될 가능성이 매우 높습니다.
    
            해결 방안:
            
            방법 A (추천):
            충돌체(Collider)는 끄지 말고,PlayerStateData에 isInvincible (무적) 플래그를 두거나
            CCombatStat 쪽에 무적 상태를 설정하여, 피격 판정 로직에서만 데미지를 무시하도록 처리해야 합니다.
    
            방법 B:
            만약 벽 충돌체와 피격 충돌체가 분리되어 있다면, 피격용 충돌체만 꺼야 합니다.
            */
            if (m_OwnerColliderCom) m_OwnerColliderCom->Set_CollisionActive(false);

            // 구를 때는 마찰력 무시
            if (m_OwnerRigidbodyCom) m_OwnerRigidbodyCom->Set_Friction(0.f);

            return;
        }
    }

    // [우선순위 2] 공격 입력 처리 (공격 시작 시 방향 보정 로직 포함)
    // - 구르기 상태여도 공격으로 이행 (캔슬)
    else if (inputMgr->MouseDown(DIM_LB))
    {
        if (curState == ACTORSTATE::PLAYER_ROLL ||
            curState == ACTORSTATE::IDLE ||
            curState == ACTORSTATE::RUN)
        {
            m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_SWORD_COMBO1);

            // 공격 시작 시에만 초기화
            m_StateData.Reset();
            m_StateData.comboIndex = 1;

            if (curState == ACTORSTATE::PLAYER_ROLL)
            {
                if (m_OwnerColliderCom) m_OwnerColliderCom->Set_CollisionActive(true);
                if (m_OwnerRigidbodyCom) m_OwnerRigidbodyCom->Set_Friction(0.2f); // 기본 마찰력 복구
            }
        }
        // TODO : PLAYER_ROLL, RUN 의 경우 대쉬 공격으로 가도록 
        else if (IsAttacking(curState))
        {
            if (m_StateData.canNextCombo)
                m_StateData.isNextComboReserved = true;

            return;
        }
    }

    // TODO : 마우스의 월드 좌표를 구해서 마우스 뱡향으로 공격이 되도록 구성하기

    //!! 공격 or 구르기 중이라면 이동/대기 상태로 넘어가지 않도록 여기서 리턴
    if (IsAttacking(curState) || curState == ACTORSTATE::PLAYER_ROLL)
        return;

    // [우선순위 3] 이동 (방향 입력)
    if (D3DXVec3LengthSq(&m_StateData.moveDir) > 0.001f)
    {
        if (curState != ACTORSTATE::RUN)
        {
            m_OwnerStateCom->Change_State(ACTORSTATE::RUN);
            m_StateData.Reset();
        }
    }

    // [우선순위 4] 대기 (입력 없음)
    else
    {
        if (curState != ACTORSTATE::IDLE)
        {
            m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
            m_StateData.Reset();
        }
    }
}

void CPlayerController::Update_State(const _float& timeDelta)
{
    ACTORSTATE curState = m_OwnerStateCom->Get_State();

    m_StateData.timer += timeDelta; // 타이머 증가

    switch (curState)
    {
    case ACTORSTATE::IDLE:
        Action_Idle(timeDelta);
        break;
    case ACTORSTATE::RUN:
        Action_Run(timeDelta);
        break;
    case ACTORSTATE::PLAYER_ROLL:
        Action_Roll(timeDelta);
        break;
    case ACTORSTATE::PLAYER_SWORD_COMBO1:
    case ACTORSTATE::PLAYER_SWORD_COMBO2:
    case ACTORSTATE::PLAYER_SWORD_COMBO3:
        Action_Sword(timeDelta);
        break;
    case ACTORSTATE::HIT:
        break;
    case ACTORSTATE::DEAD:
        break;
    default: break;
    }
}

_bool CPlayerController::CanChangeDir(ACTORSTATE state)
{
    //!! 구르기, 사망, 피격, 공격 중에는 방향 전환 불가
    if (state == ACTORSTATE::PLAYER_ROLL ||
        state == ACTORSTATE::DEAD ||
        state == ACTORSTATE::HIT ||
        state == ACTORSTATE::PLAYER_SWORD_COMBO1 ||
        state == ACTORSTATE::PLAYER_SWORD_COMBO2 ||
        state == ACTORSTATE::PLAYER_SWORD_COMBO3)
        return false;

    return true;
}

void CPlayerController::ChangeDir()
{
    // 입력 없으면 리턴
    if (D3DXVec3LengthSq(&m_StateData.moveDir) <= 0.001f) return;

    ACTORSTATE curState = m_OwnerStateCom->Get_State();

    // 현재 상태가 방향 전환을 허용하는지 체크
    if (!CanChangeDir(curState)) return;

    ACTORDIR newDir = m_OwnerStateCom->Get_Dir();
    _vec3    dir    = m_StateData.moveDir;

    // 방향 판별 로직
    if (dir.x > 0.1f)
    {
        if (dir.z > 0.1f)
            newDir = ACTORDIR::R_UP;
        else if (dir.z < -0.1f)
            newDir = ACTORDIR::R_DOWN;
        else
            newDir = ACTORDIR::RIGHT;
    }
    else if (dir.x < -0.1f)
    {
        if (dir.z > 0.1f)
            newDir = ACTORDIR::L_UP;
        else if (dir.z < -0.1f)
            newDir = ACTORDIR::L_DOWN;
        else
            newDir = ACTORDIR::LEFT;
    }
    else
    {
        if (dir.z > 0.1f)
            newDir = ACTORDIR::UP;
        else if (dir.z < -0.1f)
            newDir = ACTORDIR::DOWN;
    }

    m_OwnerStateCom->Change_Dir(newDir);
}

_vec3 CPlayerController::CalcWorldDir(ACTORDIR dir)
{
    _vec3 vDir;

    switch (dir)
    {
    case ACTORDIR::LEFT:
    default:
        vDir = { -1.f, 0.f, 0.f };
        break;
    case ACTORDIR::RIGHT:
        vDir = { 1.f, 0.f, 0.f };
        break;
    case ACTORDIR::UP:
        vDir = { 0.f, 0.f, 1.f };
        break;
    case ACTORDIR::DOWN:
        vDir = { 0.f, 0.f, -1.f };
        break;
    case ACTORDIR::L_UP:
        vDir = { -1.f, 0.f, 1.f };
        break;
    case ACTORDIR::R_UP:
        vDir = { 1.f, 0.f, 1.f };
        break;
    case ACTORDIR::L_DOWN:
        vDir = { -1.f, 0.f, -1.f };
        break;
    case ACTORDIR::R_DOWN:
        vDir = { 1.f, 0.f, -1.f };
        break;
    }

    D3DXVec3Normalize(&vDir, &vDir);
    return vDir;
}

_bool CPlayerController::IsAttacking(ACTORSTATE state)
{
    return (state == ACTORSTATE::PLAYER_SWORD_COMBO1 ||
            state == ACTORSTATE::PLAYER_SWORD_COMBO2 ||
            state == ACTORSTATE::PLAYER_SWORD_COMBO3);
}

void CPlayerController::Action_Idle(const _float& timeDelta)
{
    m_OwnerRigidbodyCom->Set_Friction(0.2f);
}

void CPlayerController::Action_Run(const _float& timeDelta)
{
    m_OwnerRigidbodyCom->Set_Friction(0.05f);

    // 1. 방향 회전
    ChangeDir();

    // 2. 물리적 이동 (Force 사용)
    _float speed = m_OwnerCombatStatCom->Get_Speed();
    m_OwnerRigidbodyCom->Add_Force(m_StateData.moveDir * speed * 20.f);
}

void CPlayerController::Action_Roll(const _float& timeDelta)
{
    // 구르기 종료
    if (m_StateData.timer >= m_StateData.rollDuration)
    {
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        m_OwnerColliderCom->Set_CollisionActive(true);
        m_OwnerRigidbodyCom->Set_UseAccel(true);
        m_OwnerRigidbodyCom->Set_Friction(0.2f);

        m_StateData.Reset();
    }
    // 구르기 중
    else
    {
        // 구르기 무적시간 종료 체크
        if (!m_StateData.isRollInvincibleEnded &&
            m_StateData.timer >= m_StateData.rollInvincibleDuration)
        {
            m_OwnerColliderCom->Set_CollisionActive(true);

            m_StateData.isRollInvincibleEnded = true;
        }

        const _vec3  rollDir   = m_StateData.rollDir;
        const _float rollSpeed = m_OwnerCombatStatCom->Get_Speed() * 3.f;

        m_OwnerRigidbodyCom->Set_UseAccel(false);

        // 구르기에 한해서는 Move_Pos로 강제 이동
        m_TransformCom->Move_Pos(rollDir, timeDelta, rollSpeed);
    }
}

void CPlayerController::Action_Sword(const _float& timeDelta)
{
    m_OwnerRigidbodyCom->Set_Friction(0.2f);

    ACTORSTATE curState = m_OwnerStateCom->Get_State();

    _float attackSpeed = 1.f;
    if (m_OwnerCombatStatCom)
        attackSpeed = m_OwnerCombatStatCom->Get_AttackSpeed();

    _float attackDuration = m_StateData.Get_SwordAttackDuration(attackSpeed);

    // 1. 공격 판정 (Hitbox) 제어
    if (m_StateData.timer >= attackDuration * 0.2f &&
        m_StateData.timer <= attackDuration * 0.6f)
    {
        if (!m_StateData.isHitboxActive)
        {
            m_StateData.isHitboxActive = true;
            // 예시 : if(m_WeaponCollider) m_WeaponCollider->Set_Active(true);

            dynamic_cast<CPlayer*>(m_Owner)->Test_Attack();
        }
    }
    else
    {
        if (m_StateData.isHitboxActive)
        {
            m_StateData.isHitboxActive = false;
            // 예시 : if(m_WeaponCollider) m_WeaponCollider->Set_Active(false);
        }
    }

    // 2. 선입력 구간 체크
    if (m_StateData.timer > attackDuration * 0.4f && m_StateData.comboIndex < 3)
        m_StateData.canNextCombo = true;

    // 3. 애니메이션 종료 시 처리
    if (m_StateData.timer >= attackDuration)
    {
        if (m_StateData.isNextComboReserved && m_StateData.comboIndex < 3)
        {
            // 다음 콤보로 전이
            ++m_StateData.comboIndex;

            ACTORSTATE nextState = static_cast<ACTORSTATE>(
                static_cast<_uint>(ACTORSTATE::PLAYER_SWORD_COMBO1)
                + m_StateData.comboIndex - 1);

            m_OwnerStateCom->Change_State(nextState);
            m_StateData.Reset();
        }
        else
        {
            // 콤보 종료 -> IDLE 복귀
            m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
            m_StateData.Reset();
            m_StateData.comboIndex = 0; // 콤보 끝났으므로 0으로 명시적 초기화
        }
    }
}

CPlayerController* CPlayerController::Create(DEVICE graphicDev)
{
    auto controller = new CPlayerController(graphicDev);

    if (FAILED(controller->Ready_PlayerController()))
    {
        Safe_Release(controller);
        MSG_BOX("PlayerController Create Failed");
        return nullptr;
    }

    return controller;
}

CComponent* CPlayerController::Clone()
{
    return new CPlayerController(*this);
}

void CPlayerController::Free()
{
    //!! Safe_Release() 등을 호출해 원본을 부수지 말고, nullptr로 빌려온 주소를 반납만 한다. 
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerRigidbodyCom  = nullptr;
    m_OwnerColliderCom   = nullptr;

    CComponent::Free();
}
