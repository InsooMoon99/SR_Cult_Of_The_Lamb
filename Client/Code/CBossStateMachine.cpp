#include "pch.h"
#include "CBossStateMachine.h"
#include "CBossPhase1.h"
#include "CMonsterState.h"

#include "CBossAwake.h"
#include "CBossBulletAttack.h"
#include "CBossChainAttack.h"
#include "CBossDead.h"
#include "CBossGroggy.h"
#include "CBossIdle.h"
#include "CBossMove.h"

CBossStateMachine::CBossStateMachine(CBossPhase1* owner) : m_pOwner(owner)
{
}

CBossStateMachine::~CBossStateMachine()
{
    Free();
}

HRESULT CBossStateMachine::Ready_StateMachine()
{
    Ready_BossStates();

    m_pCurState = m_vecState[CBossPhase1::BOSSPHASE1::BOSSAWAKE];

    m_pCurState->Enter();

    return S_OK;
}

_int CBossStateMachine::Update_StateMachine(const _float& timeDelta)
{
    // 씬메니져의 리퀘스트 씬 같은 느낌?
    if (m_pCurState)
    {
        _uint nextState = m_pCurState->Update_State(timeDelta);

        if (nextState < CBossPhase1::BOSSPHASE1::BOSS_END && nextState != m_dwCurStateKey)
        {
            Change_State(nextState);
        }
    }

    return 0; //일단 0, 형식 통일 떄문에 int로 함 void 여도 됨
}

void CBossStateMachine::LateUpdate_StateMachine(const _float& timeDelta)
{
    m_pCurState->LateUpdate_State(timeDelta);
}

void CBossStateMachine::Change_State(_uint changeStateKey)
{
    // 체인지 씬이랑 비슷한 느낌

    if (m_dwCurStateKey == changeStateKey)
        return; // 이전이랑 같은 상황

    if(m_vecState[changeStateKey] == nullptr || changeStateKey >= CBossPhase1::BOSSPHASE1::BOSS_END)
        return; // 인덱스 범위 확인

    // 이전 상태 나가기
    if (m_pCurState)
       m_pCurState->Exit();

    // 이전 상태 등록
    //m_pPrevState = m_pCurState;
    m_dwPrevStateKey = m_dwCurStateKey;
    m_dwCurStateKey = changeStateKey;

    // 새 상태로 가져오기
    m_pCurState = m_vecState[changeStateKey];
    m_pCurState->Enter();

}

_int CBossStateMachine::GetCurStateNum()
{
    { return m_pCurState->Get_StateKey(); }
}

HRESULT CBossStateMachine::Ready_BossStates()
{
    m_vecState.resize(CBossPhase1::BOSSPHASE1::BOSS_END);
    //m_vecState.resize(2);

    CMonsterState* pState = nullptr;

    if (!(pState = CBossAwake::Create(m_pOwner, this)))
        return E_FAIL;

    m_vecState[CBossPhase1::BOSSPHASE1::BOSSAWAKE] = pState;

    if (!(pState = CBossBulletAttack::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BULLETATTACK] = pState;

    if (!(pState = CBossChainAttack::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::CHAINATTACK] = pState;
    
    if (!(pState = CBossDead::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSDEAD] = pState;
    
    if (!(pState = CBossIdle::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSIDLE] = pState;
    
    if (!(pState = CBossMove::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSMOVE] = pState;
    
    if (!(pState = CBossGroggy::Create(m_pOwner, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSGROGGRY] = pState;



    return S_OK;
}


CBossStateMachine* CBossStateMachine::Create(CBossPhase1* owner)
{
    auto StateMachine = new CBossStateMachine(owner);
    if (FAILED(StateMachine->Ready_StateMachine()))
    {
        Safe_Release(StateMachine);
        MSG_BOX("StateMachine Created Failed");
        return nullptr;
    }

    return StateMachine;

}

void CBossStateMachine::Free()
{

    CStateMachine::Free();
}
