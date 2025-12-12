#include "pch.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CNPCBase.h"

CNPCController::CNPCController(DEVICE graphicDev)
    : CMonsterController(graphicDev)
{ }

CNPCController::CNPCController(const CNPCController& rhs)
    : CMonsterController(rhs)
{ }

CNPCController::~CNPCController()
{ }

HRESULT CNPCController::Ready_Controller()
{
    if (FAILED(CMonsterController::Ready_Controller()))
        return E_FAIL;

    return S_OK;
}

void CNPCController::Initialize_Component()
{
    CMonsterController::Initialize_Component();

    // StateMachine 생성 및 설정
    // NPC는 개별적으로 StateMachine을 가질 수 있으므로 여기서 생성
    CNPCStateMachine* stateMachine = CNPCStateMachine::Create();

    if (stateMachine)
    {
        // Owner 설정 (CNPCBase로 캐스팅)
        stateMachine->Set_Owner(dynamic_cast<CNPCBase*>(m_Owner));
        
        Set_StateMachine(stateMachine);
        Safe_Release(stateMachine); // Set_StateMachine에서 AddRef 함
    }
}

_int CNPCController::Update_Component(const _float& timeDelta)
{
    if (m_StateMachine)
        m_StateMachine->Update_StateMachine(timeDelta);

    return 0;
}

CNPCController* CNPCController::Create(DEVICE graphicDev)
{
    CNPCController* instance = new CNPCController(graphicDev);

    if (FAILED(instance->Ready_Controller()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

CComponent* CNPCController::Clone()
{
    return new CNPCController(*this);
}

void CNPCController::Free()
{
    CMonsterController::Free();
}
