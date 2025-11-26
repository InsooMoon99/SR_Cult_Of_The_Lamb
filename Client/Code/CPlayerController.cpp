#include "pch.h"
#include "CPlayerController.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CMainEditorMgr.h"
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



    // 입력 수집
    Update_InputData();



    // 방향 입력에 의해 이동할 때만 마찰 줄임
    // 언제 이동이 가능하냐? 구르기, 공격 등 강제 이동하는 경우 제외하고
    // 뭔가를 들고 움직이는 등의 특수한 상황에서
    if (m_InputData.hasMoveInput)
    {
        if (m_OwnerRigidbodyCom->Get_Friction() > 0.02f)
            m_OwnerRigidbodyCom->Set_Friction(0.02f);
    }
    else
        m_OwnerRigidbodyCom->Set_Friction(10.f);

    State_Roll(timeDelta);




    return 0;
}

void CPlayerController::LateUpdate_Component()
{ }

HRESULT CPlayerController::Init_OwnerComponent()
{
    m_OwnerStateCom =
        dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom =
        dynamic_cast<CCombatStat*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerRigidbodyCom =
        dynamic_cast<CRigidbody*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    m_OwnerColliderCom =
        dynamic_cast<CBoxCollider*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::BOX_COLL));

    NULL_CHECK_RETURN_MSG(
        m_OwnerStateCom,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's STATE caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerCombatStatCom,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's COMBATSTAT caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerRigidbodyCom,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's RIGIDBODY caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerColliderCom,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's BOXCOLLIDER caching has failed");

    return S_OK;
}

void CPlayerController::Update_InputData()
{
    const auto inputMgr = CDInputMgr::GetInstance();

    // KeyInputData 값 초기화
    m_InputData.axisX = 0.f;
    m_InputData.axisZ = 0.f;

    // Z축 (W, S)
    if (inputMgr->Get_DIKeyState(DIK_W) & 0x80 || inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
        ++m_InputData.axisZ;
    if (inputMgr->Get_DIKeyState(DIK_S) & 0x80 || inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
        --m_InputData.axisZ;

    // X축 (A, D)
    if (inputMgr->Get_DIKeyState(DIK_D) & 0x80 || inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
        ++m_InputData.axisX;
    if (inputMgr->Get_DIKeyState(DIK_A) & 0x80 || inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
        --m_InputData.axisX;

    // 입력 존재 여부 플래그 갱신
    if (D3DXVec3LengthSq(&m_InputData.moveDir) > 0.f)
        m_InputData.hasMoveInput = true;
    else
        m_InputData.hasMoveInput = false;
}

void CPlayerController::Update_State()
{
    
}

void CPlayerController::Update_Dir()
{
    ACTORDIR curDir     = m_OwnerStateCom->Get_Dir();
    m_InputData.moveDir = { 0.f, 0.f, 0.f };

    switch (curDir)
    {
    case ACTORDIR::LEFT:    m_InputData.moveDir = -g_WorldRight; break;
    case ACTORDIR::RIGHT:   m_InputData.moveDir = g_WorldRight; break;
    case ACTORDIR::UP:      m_InputData.moveDir = g_WorldLook; break;
    case ACTORDIR::DOWN:    m_InputData.moveDir = -g_WorldLook; break;

    case ACTORDIR::L_UP:    m_InputData.moveDir = -g_WorldRight + g_WorldLook; break;
    case ACTORDIR::R_UP:    m_InputData.moveDir = g_WorldRight + g_WorldLook; break;
    case ACTORDIR::R_DOWN:  m_InputData.moveDir = g_WorldRight - g_WorldLook; break;
    case ACTORDIR::L_DOWN:  m_InputData.moveDir = -g_WorldRight - g_WorldLook; break;

    default:                m_InputData.moveDir = -g_WorldRight; break;
    }

    D3DXVec3Normalize(&m_InputData.moveDir, &m_InputData.moveDir);
}

void CPlayerController::ChangeDirState()
{
    if (m_InputData.axisX == 0 && m_InputData.axisZ == 0)
        return;

    ACTORDIR newDir     = m_OwnerStateCom->Get_Dir();

    // 1. 입력에 따라 방향(Enum) 바꾸기
    if (m_InputData.axisZ > 0)
    {
        if (m_InputData.axisX > 0)      newDir = ACTORDIR::R_UP;
        else if (m_InputData.axisX < 0) newDir = ACTORDIR::L_UP;
        else                            newDir = ACTORDIR::UP;
    }
    else if (m_InputData.axisZ < 0)
    {
        if (m_InputData.axisX > 0)      newDir = ACTORDIR::R_DOWN;
        else if (m_InputData.axisX < 0) newDir = ACTORDIR::L_DOWN;
        else                            newDir = ACTORDIR::DOWN;
    }
    else
    {
        if (m_InputData.axisX > 0)      newDir = ACTORDIR::RIGHT;
        else if (m_InputData.axisX < 0) newDir = ACTORDIR::LEFT;
    }

    m_OwnerStateCom->Change_Dir(newDir);
}



void CPlayerController::Key_Input(const _float& timeDelta)
{
    ACTORSTATE currentState = m_OwnerStateCom->Get_State();

    // 1. 가능한 경우 방향(Direction) 결정
    if (CanApplyDir(currentState))
        //Apply_Direction();

    // 2. 상태(State) 결정
    Determine_State();

    currentState = m_OwnerStateCom->Get_State();

    // 3. 가능한 경우 이동(Position) 적용
    if (CanApplyMovement(currentState))
        Apply_Movement(timeDelta);
}

void CPlayerController::State_Roll(const _float& timeDelta)
{
    m_RollData.rollElapsedTime += timeDelta;

    // IDLE로 돌아가기
    if (m_RollData.rollElapsedTime >= m_RollData.rollDuration)
    {
        m_RollData.rollElapsedTime = 0.f;

        m_TransformCom->Set_Scale(1.f, 1.f, 1.f);
        m_OwnerRigidbodyCom->Set_Friction(true);

        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        m_OwnerColliderCom->Set_CollisionActive(true); // 충돌체 키기
    }
    // ROLL 하는 중
    else
    {
        const _float rollSpeed = m_OwnerCombatStatCom->Get_Speed() * 3.5f; // 구르기 속도 배율

        // 마찰이 필요 없는 강제 이동이므로 rigidbody의 마찰 및 AddForce 사용 안함
        m_OwnerRigidbodyCom->Set_Friction(false);
        m_TransformCom->Move_Pos(m_InputData.moveDir, timeDelta, rollSpeed);

        m_OwnerColliderCom->Set_CollisionActive(false); // 충돌체 끄기
    }
}

_bool CPlayerController::CanApplyDir(ACTORSTATE currentState)
{
    //!! ROLL, ATTACK, DEAD 에서 방향 변경 금지
    if (ACTORSTATE::PLAYER_ROLL == currentState ||
        ACTORSTATE::ATTACK == currentState ||
        ACTORSTATE::DEAD == currentState)
        return false;

    return true;
}

void CPlayerController::Determine_State()
{
    if (nullptr == m_OwnerStateCom)
        return;

    ACTORSTATE nextState = ACTORSTATE::IDLE;

    //!! 현재 우선순위: ROLL > ATTACK > RUN > IDLE

    const auto inputMgr = CDInputMgr::GetInstance();

    // 구르기 상태 진입 체크
    if (!m_RollData.isRolling &&
        (inputMgr->Get_DIKeyState(DIK_SPACE) & 0x80))
    {

        m_RollData.rollElapsedTime = 0.f;

        


        nextState = ACTORSTATE::PLAYER_ROLL;
    }

    // TODO 구르기 중

    else if (m_InputData.hasMoveInput)
        nextState = ACTORSTATE::RUN;

    else
        nextState = ACTORSTATE::IDLE;

    // 상태 변경 요청
    m_OwnerStateCom->Change_State(nextState);
}

_bool CPlayerController::CanApplyMovement(ACTORSTATE currentState)
{
    //!! ROLL, DEAD 에서 수동 이동 금지
    if (ACTORSTATE::PLAYER_ROLL == currentState ||
        ACTORSTATE::DEAD == currentState)
        return false;

    return true;
}

void CPlayerController::Apply_Movement(const _float& timeDelta)
{
    if (!m_InputData.hasMoveInput)
        return;

    ACTORSTATE curState   = m_OwnerStateCom->Get_State();
    float      finalSpeed = m_OwnerCombatStatCom->Get_Speed();

    D3DXVec3Normalize(&m_InputData.moveDir, &m_InputData.moveDir);

    // 가속 없는 등속 이동
    //m_TransformCom->Move_Pos(m_InputData.moveDir, timeDelta, finalSpeed);

    // 가속 이동. 입력이 지속될수록 가속력 증가, 최대 속도 상한까지 계속 올라감
    m_OwnerRigidbodyCom->Add_Force(m_InputData.moveDir * finalSpeed * 3.f);
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
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerRigidbodyCom  = nullptr;

    CComponent::Free();
}
