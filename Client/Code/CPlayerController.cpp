#include "pch.h"
#include "CPlayerController.h"

#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CPlayerController::CPlayerController(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStat(nullptr),
      m_OwnerRigidbody(nullptr)
{ }

CPlayerController::CPlayerController(const CPlayerController& rhs)
    : CComponent(rhs),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStat(nullptr),
      m_OwnerRigidbody(nullptr)
{ }

CPlayerController::~CPlayerController()
{ }

HRESULT CPlayerController::Ready_PlayerController()
{
    return S_OK;
}

_int CPlayerController::Update_Component(const _float& timeDelta)
{
    if (m_RollData.isRolling)
        Update_Roll(timeDelta);
    else
        Key_Input(timeDelta);

    return 0;
}

void CPlayerController::LateUpdate_Component()
{ }

HRESULT CPlayerController::Init_OwnerComponent()
{
    m_OwnerStateCom =
        dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStat =
        dynamic_cast<CCombatStat*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerRigidbody =
        dynamic_cast<CRigidbody*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    NULL_CHECK_RETURN_MSG(
        m_OwnerStateCom,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's STATE caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerCombatStat,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's COMBATSTAT caching has failed");

    NULL_CHECK_RETURN_MSG(
        m_OwnerRigidbody,
        E_FAIL,
        L"CPlayerController::Init_OwnerComponent(): Owner's RIGIDBODY caching has failed");

    return S_OK;
}

void CPlayerController::Key_Input(const _float& timeDelta)
{
    KeyInputData inputData; // 매 프레임 입력 저장값 초기화
    ACTORSTATE   currentState = m_OwnerStateCom->Get_State();

    // 1. 입력 데이터 수집
    Calc_InputVector(inputData);

    // 2. 가능한 경우 방향(Direction) 결정
    if (CanApplyDirection(currentState))
        Apply_Direction(inputData);

    // 3. 상태(State) 결정
    Determine_State(inputData);

    currentState = m_OwnerStateCom->Get_State();

    // 4. 가능한 경우 이동(Position) 적용
    if (CanApplyMovement(currentState))
        Apply_Movement(inputData, timeDelta);
}

void CPlayerController::Update_Roll(const _float& timeDelta)
{
    if (nullptr == m_TransformCom ||
        nullptr == m_OwnerStateCom ||
        nullptr == m_OwnerCombatStat ||
        nullptr == m_OwnerRigidbody)
        return;

    m_RollData.rollElapsedTime += timeDelta;

    // IDLE로 돌아가기
    if (m_RollData.rollElapsedTime >= m_RollData.rollDuration)
    {
        m_RollData.isRolling       = false;
        m_RollData.rollElapsedTime = 0.f;

        m_TransformCom->Set_Scale(1.f, 1.f, 1.f);
        m_OwnerRigidbody->Set_Friction(true);

        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
    }
    // ROLL 하는 중
    else
    {
#pragma region 구르기 텍스처 크기 문제로 임의로 스케일 값 조정
        // TODO 임시로 스케일 값 조정중. 고쳐지면 수정할 것
        //!! 스케일을 키우면 collider의 크기 또한 커지는 문제 있음

        if (m_OwnerStateCom->Get_Dir() == ACTORDIR::LEFT ||
            m_OwnerStateCom->Get_Dir() == ACTORDIR::L_UP ||
            m_OwnerStateCom->Get_Dir() == ACTORDIR::L_DOWN)
            m_TransformCom->Set_Scale(1.4f, 1.4f, 1.f);

        else if (m_OwnerStateCom->Get_Dir() == ACTORDIR::RIGHT ||
                 m_OwnerStateCom->Get_Dir() == ACTORDIR::R_UP ||
                 m_OwnerStateCom->Get_Dir() == ACTORDIR::R_DOWN)
            m_TransformCom->Set_Scale(-1.4f, 1.4f, 1.f);

        else if (m_OwnerStateCom->Get_Dir() == ACTORDIR::UP ||
                 m_OwnerStateCom->Get_Dir() == ACTORDIR::DOWN)
            m_TransformCom->Set_Scale(0.88f, 0.88f, 1.f);

#pragma endregion

        const _float rollSpeed = m_OwnerCombatStat->Get_Speed() * 2.5f; // 구르기 속도 배율

        // 마찰이 필요 없는 강제 이동이므로 rigidbody의 마찰 및 AddForce 사용 안함
        m_OwnerRigidbody->Set_Friction(false);
        m_TransformCom->Move_Pos(m_RollData.rollDir, timeDelta, rollSpeed);

        //m_OwnerRigidbody->Add_Force(m_RollData.rollDir * rollSpeed * 5.f);
    }
}

void CPlayerController::Calc_InputVector(KeyInputData& inputData)
{
    const auto inputMgr = CDInputMgr::GetInstance();

    // Z축
    if (inputMgr->Get_DIKeyState(DIK_W) & 0x80 || inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
    {
        inputData.moveDir += g_WorldLook;
        ++inputData.axisZ;
    }
    if (inputMgr->Get_DIKeyState(DIK_S) & 0x80 || inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
    {
        inputData.moveDir -= g_WorldLook;
        --inputData.axisZ;
    }

    // X축
    if (inputMgr->Get_DIKeyState(DIK_D) & 0x80 || inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
    {
        inputData.moveDir += g_WorldRight;
        ++inputData.axisX;
    }
    if (inputMgr->Get_DIKeyState(DIK_A) & 0x80 || inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
    {
        inputData.moveDir -= g_WorldRight;
        --inputData.axisX;
    }

    inputData.isMove = (D3DXVec3LengthSq(&inputData.moveDir) > 0.f);
}

_bool CPlayerController::CanApplyDirection(ACTORSTATE currentState)
{
    //!! ROLL, ATTACK, DEAD 에서 방향 변경 금지
    if (ACTORSTATE::ROLL == currentState ||
        ACTORSTATE::ATTACK == currentState ||
        ACTORSTATE::DEAD == currentState)
        return false;

    return true;
}

void CPlayerController::Apply_Direction(KeyInputData& inputData)
{
    if (nullptr == m_OwnerStateCom)
        return;

    // 입력이 아예 없으면 방향을 바꾸지 않음 (이전 방향 유지)
    if (inputData.axisX == 0 && inputData.axisZ == 0)
        return;

    ACTORDIR newDir = ACTORDIR::DOWN; // 기본값

    if (inputData.axisZ > 0) // 상단
    {
        if (inputData.axisX > 0)
            newDir = ACTORDIR::R_UP;
        else if (inputData.axisX < 0)
            newDir = ACTORDIR::L_UP;
        else
            newDir = ACTORDIR::UP;
    }
    else if (inputData.axisZ < 0) // 하단
    {
        if (inputData.axisX > 0)
            newDir = ACTORDIR::R_DOWN;
        else if (inputData.axisX < 0)
            newDir = ACTORDIR::L_DOWN;
        else
            newDir = ACTORDIR::DOWN;
    }
    else // 중단 (Z축 입력 없음)
    {
        if (inputData.axisX > 0)
            newDir = ACTORDIR::RIGHT;
        else if (inputData.axisX < 0)
            newDir = ACTORDIR::LEFT;
    }

    m_OwnerStateCom->Change_Dir(newDir);
}

void CPlayerController::Determine_State(KeyInputData& inputData)
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
        m_RollData.isRolling       = true;
        m_RollData.rollElapsedTime = 0.f;

        switch (m_OwnerStateCom->Get_Dir())
        {
        case ACTORDIR::RIGHT:
            m_RollData.rollDir = g_WorldRight;
            break;
        case ACTORDIR::UP:
            m_RollData.rollDir = g_WorldLook;
            break;
        case ACTORDIR::DOWN:
            m_RollData.rollDir = - g_WorldLook;
            break;
        case ACTORDIR::L_UP:
            m_RollData.rollDir = - g_WorldRight + g_WorldLook;
            break;
        case ACTORDIR::R_UP:
            m_RollData.rollDir = g_WorldRight + g_WorldLook;
            break;
        case ACTORDIR::L_DOWN:
            m_RollData.rollDir = - g_WorldRight - g_WorldLook;
            break;
        case ACTORDIR::R_DOWN:
            m_RollData.rollDir = g_WorldRight - g_WorldLook;
            break;
        default:
            m_RollData.rollDir = - g_WorldRight;
            break;
        }

        D3DXVec3Normalize(&m_RollData.rollDir, &m_RollData.rollDir);

        nextState = ACTORSTATE::ROLL;
    }

    // 구르기 중
    else if (m_RollData.isRolling)
        return;

    else if (inputData.isMove)
        nextState = ACTORSTATE::RUN;

    else
        nextState = ACTORSTATE::IDLE;

    // 상태 변경 요청
    m_OwnerStateCom->Change_State(nextState);
}

_bool CPlayerController::CanApplyMovement(ACTORSTATE currentState)
{
    //!! ROLL, DEAD 에서 수동 이동 금지
    if (ACTORSTATE::ROLL == currentState ||
        ACTORSTATE::DEAD == currentState)
        return false;

    return true;
}

void CPlayerController::Apply_Movement(KeyInputData& inputData, const _float& timeDelta)
{
    if (!inputData.isMove)
        return;

    if (nullptr == m_TransformCom ||
        nullptr == m_OwnerCombatStat ||
        nullptr == m_OwnerStateCom ||
        nullptr == m_OwnerRigidbody)
        return;

    ACTORSTATE curState   = m_OwnerStateCom->Get_State();
    float      finalSpeed = m_OwnerCombatStat->Get_Speed();

    // 상태에 따라 이동 속도 제한
    switch (curState)
    {
    case ACTORSTATE::ATTACK:
        finalSpeed *= 0.5f;
        break;
    default:
        break;
    }

    D3DXVec3Normalize(&inputData.moveDir, &inputData.moveDir);

    // 가속 없는 등속 이동
    //m_TransformCom->Move_Pos(inputData.moveDir, timeDelta, finalSpeed);

    // 가속 이동. 입력이 지속될수록 가속력 증가, 최대 속도 상한까지 계속 올라감
    m_OwnerRigidbody->Add_Force(inputData.moveDir * finalSpeed * 4.f);
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
    m_OwnerStateCom   = nullptr;
    m_OwnerCombatStat = nullptr;
    m_OwnerRigidbody  = nullptr;

    CComponent::Free();
}
