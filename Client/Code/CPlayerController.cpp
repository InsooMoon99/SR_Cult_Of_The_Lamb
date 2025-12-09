#include "pch.h"
#include "CPlayerController.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CMainEditorMgr.h"
#include "CPlayerAttack.h"
#include "CPlayerConstruction.h"
#include "CPlayerGathering.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CPlayerSkill.h"
#include "CRigidbody.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CWorldInteractive.h"

CPlayerController::CPlayerController(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),
      m_OwnerColliderCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_OwnerMovementCom(nullptr),
      m_OwnerAttackCom(nullptr),
      m_OwnerSkillCom(nullptr),
      m_OwnerGatherCom(nullptr),
      m_OwnerConstructionCom(nullptr)
{
    ZeroMemory(m_Timers, sizeof(m_Timers));
}

CPlayerController::CPlayerController(const CPlayerController& rhs)
    : CComponent(rhs),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),
      m_OwnerColliderCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_OwnerMovementCom(nullptr),
      m_OwnerAttackCom(nullptr),
      m_OwnerSkillCom(nullptr),
      m_OwnerGatherCom(nullptr),
      m_OwnerConstructionCom(nullptr)
{
    ZeroMemory(m_Timers, sizeof(m_Timers));
}

CPlayerController::~CPlayerController()
{ }

HRESULT CPlayerController::Ready_PlayerController()
{
    return S_OK;
}

void CPlayerController::Initialize_Component()
{
    m_Timers[TIMER_RUNSOUND] = FLT_MAX;
}

_int CPlayerController::Update_Component(const _float& timeDelta)
{
    // 일시정지 -> 입력 무시
    if (!CMainEditorMgr::GetInstance()->IsPlaying())
        return -1;

    // 1. 입력 수집 (PlayerInput 컴포넌트가 자동으로 수행함)
    // Update_Input();

    // 2. 상태 전환 판단
    CheckStateTransition();

    // 3. 현재 상태에 따른 액션 수행
    Update_State(timeDelta);

    return 0;
}

void CPlayerController::LateUpdate_Component()
{}

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

    m_OwnerInputCom = dynamic_cast<CPlayerInput*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_INPUT));

    m_OwnerMovementCom = dynamic_cast<CPlayerMovement*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::PL_MOVEMENT));

    m_OwnerAttackCom = dynamic_cast<CPlayerAttack*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::PL_ATTACK));

    m_OwnerSkillCom = dynamic_cast<CPlayerSkill*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::PL_SKILL));

    m_OwnerGatherCom = dynamic_cast<CPlayerGathering*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::GATHERING));

    m_OwnerConstructionCom = dynamic_cast<CPlayerConstruction*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::CONSTRUCTION));

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

    NULL_CHECK_RETURN_MSG(
        m_OwnerInputCom,
        E_FAIL,
        L"CPlayerController: Owner's INPUT caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerMovementCom,
        E_FAIL,
        L"CPlayerController: Owner's MOVEMENT caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerAttackCom,
        E_FAIL,
        L"CPlayerController: Owner's ATTACK caching has failed");

    return S_OK;
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
            // 공격 중이었다면 공격 취소
            if (curState == ACTORSTATE::PLAYER_SWORD_COMBO1 ||
                curState == ACTORSTATE::PLAYER_SWORD_COMBO2 ||
                curState == ACTORSTATE::PLAYER_SWORD_COMBO3 ||
                curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO1 ||
                curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO2 ||
                curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO3 ||
                curState == ACTORSTATE::PLAYER_GAUNTLET_HEAVY)
                m_OwnerAttackCom->Cancel_Attack();

            _vec3 moveDir = m_OwnerInputCom->Get_MoveDir();
            m_OwnerMovementCom->ChangeActorDir(moveDir);

            // 이동 중이 아니면 바라보는 방향으로 구르기
            if (D3DXVec3LengthSq(&moveDir) <= 0.f)
                moveDir = m_OwnerMovementCom->ActorDirToVec(m_OwnerStateCom->Get_Dir());

            // 구르기 시작
            m_OwnerMovementCom->Start_Roll(moveDir);
            m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_ROLL);
            return;
        }
    }

    // [우선순위 2] 좌클릭
    // - 구르기 상태여도 공격으로 이행 (캔슬)
    else if (inputMgr->MouseDown(DIM_LB))
    {
        // ImGui 클릭 시 무시
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            return;

        // 좌클릭으로 채집, 건축
        if (Living_Interaction())
            return;

        if (curState == ACTORSTATE::PLAYER_ROLL ||
            curState == ACTORSTATE::IDLE ||
            curState == ACTORSTATE::RUN)
        {
            ACTORSTATE nextState = m_OwnerAttackCom->Start_NormalAttack();
            if (nextState != ACTORSTATE::STATE_END)
                m_OwnerStateCom->Change_State(nextState);
        }
        // 공격 중이라면 콤보 예약 시도
        else if (curState == ACTORSTATE::PLAYER_SWORD_COMBO1 ||
                 curState == ACTORSTATE::PLAYER_SWORD_COMBO2 ||
                 curState == ACTORSTATE::PLAYER_SWORD_COMBO3 ||
                 curState == ACTORSTATE::PLAYER_SWORD_COMBO3 ||
                 curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO1 ||
                 curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO2 ||
                 curState == ACTORSTATE::PLAYER_GAUNTLET_COMBO3)

        {
            m_OwnerAttackCom->Try_NextCombo();
            return;
        }
    }

    else if (inputMgr->MouseDown(DIM_RB))
    {
        // cout << "First time Pressed" << endl;

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            return;

        if (curState == ACTORSTATE::PLAYER_ROLL ||
            curState == ACTORSTATE::IDLE ||
            curState == ACTORSTATE::RUN)
        {
            ACTORSTATE nextState = m_OwnerSkillCom->Start_Charging();
            if (nextState != ACTORSTATE::STATE_END)
                m_OwnerStateCom->Change_State(nextState);
        }
    }

    else if (inputMgr->MouseUp(DIM_RB))
    {
        if (curState == ACTORSTATE::PLAYER_SKILL_CHARGING)
        {
            // 슬로우모션 해제
            CMainEditorMgr::GetInstance()->Set_TimeScale(1.f);

            m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
            if (m_OwnerSkillCom->Get_TripleSkillAvailability())
            {
                ACTORSTATE nextState = m_OwnerSkillCom->Start_TripleSkill();
                if (nextState != ACTORSTATE::STATE_END)
                    m_OwnerStateCom->Change_State(nextState);
            }
            if (m_OwnerSkillCom->Get_SingleSkillAvailability())
            {
                ACTORSTATE nextState = m_OwnerSkillCom->Start_SingleSkill();
                if (nextState != ACTORSTATE::STATE_END)
                    m_OwnerStateCom->Change_State(nextState);
            }
            else
                m_OwnerSkillCom->Cancel_Attack();
        }
    }

    // [우선순위 3] E 키
    else if (inputMgr->KeyPress(DIK_E))
    {
        if (Living_Interaction())
            return;
    }

    // 다른 행동 중이라면 여기서 리턴
    if (!(curState == ACTORSTATE::RUN) &&
        !(curState == ACTORSTATE::IDLE))
    {
        m_OwnerSkillCom->Set_SingleSkill(false);
        m_OwnerSkillCom->Set_TripleSkill(false);

        return;
    }

    // 이동 (방향 입력)
    if (D3DXVec3LengthSq(&m_OwnerInputCom->Get_MoveDir()) > 0.001f)
    {
        if (curState != ACTORSTATE::RUN)
            m_OwnerStateCom->Change_State(ACTORSTATE::RUN);
    }

    // [우선순위 마지막] 대기 (입력 없음)
    else
    {
        if (curState != ACTORSTATE::IDLE)
        {
            m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
            m_OwnerMovementCom->Stop();
        }
    }
}

void CPlayerController::Update_State(const _float& timeDelta)
{
    ACTORSTATE curState = m_OwnerStateCom->Get_State();

    switch (curState)
    {
    case ACTORSTATE::IDLE:
    {
        Action_Idle(timeDelta);
    }
    break;

    case ACTORSTATE::RUN:
    {
        Action_Run(timeDelta);
    }
    break;

    case ACTORSTATE::PLAYER_ROLL:
    {
        Action_Roll(timeDelta);
    }
    break;

    case ACTORSTATE::PLAYER_SWORD_COMBO1:
    case ACTORSTATE::PLAYER_SWORD_COMBO2:
    case ACTORSTATE::PLAYER_SWORD_COMBO3:
    case ACTORSTATE::PLAYER_GAUNTLET_COMBO1:
    case ACTORSTATE::PLAYER_GAUNTLET_COMBO2:
    case ACTORSTATE::PLAYER_GAUNTLET_COMBO3:
    {
        Action_NormalAttack(timeDelta);
    }
    break;

    case ACTORSTATE::PLAYER_SWORD_HEAVY:
    case ACTORSTATE::PLAYER_GAUNTLET_HEAVY:
    {
        //TODO 강공격 호출. 내부에서 무기에 따라 다른 히트박스가 생성되도록 하자
    }
    break;
    case ACTORSTATE::PLAYER_SKILL_CHARGING:
        Action_Charging(timeDelta);
        break;
    case ACTORSTATE::PLAYER_SKILL_SINGLE:
        Action_SingleSkill(timeDelta);
        break;
    case ACTORSTATE::PLAYER_SKILL_TRIPLE:
        Action_TripleSkill(timeDelta);
        break;
    case ACTORSTATE::HIT:
    {
        //TODO 피격 상태 및 애니메이션이 필요할 경우 구성
    }
    break;

    case ACTORSTATE::PLAYER_BERRY:
    case ACTORSTATE::PLAYER_WOOD:
    case ACTORSTATE::PLAYER_STONE:
    {
        //if (CDInputMgr::GetInstance()->MouseUp(DIM_LB)/*&& m_Target */ )
        //m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        //if(m_OwnerGatherCom->GetInteractiveObj()!=nullptr)
        m_OwnerGatherCom->Update_Component(timeDelta);
        //else
        //    m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        // 이건 업데이트 안에서 ㄱㄱ
    }

    break;

    case ACTORSTATE::PLAYER_HAMMER:
    {
        Action_Construction(timeDelta);
    }
    break;

    case ACTORSTATE::PLAYER_WEAPONGET:
    {
        auto animator = dynamic_cast<CAnimator*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

        if (animator && animator->Is_AnimationFinished())
            m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
    }
    break;

    case ACTORSTATE::DEAD:
    {
        MSG_BOX("CPlayerController: Player Dead!");
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
    }
    break;

    case ACTORSTATE::STATE_END:
    {
        MSG_BOX("CPlayerController: Warning! Player State is STATE_END");
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
    }
    break;

    default:
    {
        MSG_BOX("CPlayerController: Invalid Player State");
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
    }
    break;
    }
}

void CPlayerController::Action_Idle(const _float& timeDelta)
{
    m_OwnerMovementCom->Stop();
}

void CPlayerController::Action_Run(const _float& timeDelta)
{
#pragma region 플레이어 달리기 효과음
    m_Timers[TIMER_RUNSOUND] += timeDelta;

    if (m_Timers[TIMER_RUNSOUND] >= 0.2f)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Walk2.wav", SOUND_EFFECT, 0.4f);
        m_Timers[TIMER_RUNSOUND] = 0.f;
    }
#pragma endregion

    m_OwnerMovementCom->Run(m_OwnerInputCom->Get_MoveDir(), timeDelta);
}

void CPlayerController::Action_Roll(const _float& timeDelta)
{
    // 구르기 업데이트 (false 반환 시 종료)
    if (!m_OwnerMovementCom->Update_Roll(timeDelta))
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
}

void CPlayerController::Action_NormalAttack(const _float& timeDelta)
{
    //m_OwnerMovementCom->Stop();

    // 일반 공격 업데이트 (false 반환 시 종료)
    if (!m_OwnerAttackCom->Update_NormalAttack(timeDelta))
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
}

void CPlayerController::Action_Charging(const _float& timeDelta)
{
    m_OwnerMovementCom->Stop();

    ACTORSTATE result = m_OwnerSkillCom->Update_Charging(timeDelta);

    switch (result)
    {
    case ACTORSTATE::IDLE:
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        break;

    case ACTORSTATE::PLAYER_SKILL_SINGLE:
        //  m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_SKILL_SINGLE);
        m_OwnerSkillCom->Set_SingleSkill(true);
        break;

    case ACTORSTATE::PLAYER_SKILL_TRIPLE:
        m_OwnerSkillCom->Set_TripleSkill(true);
        m_OwnerSkillCom->Set_SingleSkill(false);
    //  m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_SKILL_TRIPLE);
        break;

    default: break;
    }
}

void CPlayerController::Action_SingleSkill(const _float& timeDelta)
{
    if (!m_OwnerSkillCom->Update_SingleSkill(timeDelta))
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);

    //아니면 TripleSkill로 넘기는 것도 여기서 해줘야 하는 건가?
}

void CPlayerController::Action_TripleSkill(const _float& timeDelta)
{
    if (!m_OwnerSkillCom->Update_TripleSkill(timeDelta))
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
}

void CPlayerController::Action_Construction(const _float& timeDelta)
{
    bool bStopConstruction = CDInputMgr::GetInstance()->KeyUp(DIK_E) || CDInputMgr::GetInstance()->MouseUp(DIM_LB);

    if (!bStopConstruction)
    {
        if (!m_OwnerConstructionCom || !m_OwnerConstructionCom->Get_ConstructZone())
            bStopConstruction = true;
        else
            m_OwnerConstructionCom->Action_Construction(timeDelta);
    }

    if (bStopConstruction)
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
}

_bool CPlayerController::Living_Interaction()
{
    // 채집 상호작용
    if (m_OwnerGatherCom->GetInteractiveObj() != nullptr)
    {
        switch (m_OwnerGatherCom->GetInteractiveObj()->GetInteractiveType())
        {
        case CWorldInteractive::TYPE::BERRY:
        {
            if (m_OwnerStateCom->Get_State() != ACTORSTATE::PLAYER_BERRY)
                m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_BERRY);
            m_OwnerMovementCom->Stop();
            return true;
        }
        case CWorldInteractive::TYPE::WOOD:
        {
            if (m_OwnerStateCom->Get_State() != ACTORSTATE::PLAYER_WOOD)
                m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_WOOD);
            m_OwnerMovementCom->Stop();
            return true;
        }
        case CWorldInteractive::TYPE::ROCK:
        {
            if (m_OwnerStateCom->Get_State() != ACTORSTATE::PLAYER_STONE)
                m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_STONE);
            m_OwnerMovementCom->Stop();
            return true;
        }
        default: break;
        }
    }

    // 건축 상호작용
    if (m_OwnerConstructionCom && m_OwnerConstructionCom->Get_ConstructZone())
    {
        if (m_OwnerStateCom->Get_State() != ACTORSTATE::PLAYER_HAMMER)
            m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_HAMMER);
        m_OwnerMovementCom->Stop();
        return true;
    }

    return false;
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
    m_OwnerStateCom        = nullptr;
    m_OwnerCombatStatCom   = nullptr;
    m_OwnerRigidbodyCom    = nullptr;
    m_OwnerColliderCom     = nullptr;
    m_OwnerInputCom        = nullptr;
    m_OwnerMovementCom     = nullptr;
    m_OwnerAttackCom       = nullptr;
    m_OwnerSkillCom        = nullptr;
    m_OwnerConstructionCom = nullptr;
    m_OwnerGatherCom       = nullptr;

    CComponent::Free();
}
