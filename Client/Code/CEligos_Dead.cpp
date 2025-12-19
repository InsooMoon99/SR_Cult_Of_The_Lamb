#include "pch.h"
#include "CEligos_Dead.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CStage_Dungeon.h"

CEligos_Dead::CEligos_Dead(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Dead::~CEligos_Dead()
{

}

void CEligos_Dead::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_start", ANIMSTATE::ONCE);

    CStage_Dungeon* stage = dynamic_cast<CStage_Dungeon*>(CManagement::GetInstance()->Get_Scene());
    if (stage)
    {
        _vec3 pos = m_pOwnerTransformCom->Get_Pos();
        stage->Spawn_FollowerCow_Rescue(&pos);
        stage->Spawn_FollowerElephant_Rescue(&pos);
        stage->Spawn_FollowerPig_Rescue(&pos);
    }
    
    m_pOwner->Mark_As_PendingKill();

    m_ePhase = PHASE::CHANGE;
}

void CEligos_Dead::Exit()
{

    CMonsterState::Exit();
}

HRESULT CEligos_Dead::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Dead::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::BOSSDEAD;


    if (m_Sound == false)
    {
        CSoundMgr::GetInstance()->Stop_All();
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Elligos_Dead.wav", SOUNDTYPE::SOUND_BOSS2, 1.f);

        m_Sound = true;
    }

    return CEligos::ELIGOSSTATE::BOSSDEAD;
}

void CEligos_Dead::LateUpdate_State(const _float& timeDelta)
{
}

CEligos_Dead* CEligos_Dead::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Dead(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Dead::Free()
{

}
