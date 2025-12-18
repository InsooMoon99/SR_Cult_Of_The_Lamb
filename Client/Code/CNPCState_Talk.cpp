#include "pch.h"
#include "CNPCState_Talk.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CTransform.h"

CNPCState_Talk::CNPCState_Talk(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine),
      m_Timer(0.f)
{ }

CNPCState_Talk::~CNPCState_Talk()
{ }

void CNPCState_Talk::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Pray";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    _vec3 dirVec = m_PlayerTransform->Get_Pos() - m_OwnerTransform->Get_Pos();
    D3DXVec3Normalize(&dirVec, &dirVec);

    // 플레이어 쪽을 바라보면서 애니메이션 시작
    // dirVec는 PlayerPos - NPCPos. 즉 NPC -> Player 벡터
    if (m_Owner)
    {
        auto pState = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
        if (pState)
        {
            if (dirVec.x > 0.f)
                pState->Change_Dir(ACTORDIR::RIGHT);
            else
                pState->Change_Dir(ACTORDIR::LEFT);
        }
    }

    CSoundMgr::GetInstance()->Play_Once(L"Follower/follower_talk8.wav", SOUND_EFFECT, 0.5f);
}

_int CNPCState_Talk::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::TALK);
}

void CNPCState_Talk::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Talk::Ready_State()
{
    return S_OK;
}

CNPCState_Talk* CNPCState_Talk::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Talk(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Talk::Free()
{
    CNPCState::Free();
}
