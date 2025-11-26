#include "pch.h"
#include "CMonsterState.h"
#include "CManagement.h"
#include "CPlayer.h"
#include "CTransform.h"
#include "CAnimator.h"

CMonsterState::CMonsterState()
{
}

CMonsterState::~CMonsterState()
{
}

void CMonsterState::Enter()
{
    m_bStarted = true;
    m_bExited = false;
    m_bCanTransit = false;

    m_fCurStateElapsedTime = 0.f;
    m_bMinStateTimePassed = false;
}

void CMonsterState::Exit()
{
    m_bStarted = false;
    m_bExited = true;
}

_int CMonsterState::Update_State(const _float& timeDelta)
{
 

    return 0;
}

void CMonsterState::Find_Player()
{
    if (nullptr != m_pPlayer)
        return;

    CGameObject* pObject;
    CManagement::GetInstance()->Get_PersistentObject(OBJTYPE::PLAYER, &pObject);

    if (nullptr == dynamic_cast<CPlayer*>(pObject))
    {
        MSG_BOX("MonsterState can't find player!");
        return;
        
    }
    m_pPlayer = static_cast<CPlayer*>(pObject);

    CComponent* pComponent = m_pPlayer->Get_Component(COMPONENTID::ID_DYNAMIC,COMPONENTTYPE::TRANSFORM);
//CComponent* pComponent = CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Transform_Com");
    if (nullptr == dynamic_cast<CTransform*>(pComponent))
    {
        MSG_BOX("MonsterState can't find player's transform!");
        return;
    }
    m_pPlayerTransformCom = static_cast<CTransform*>(pComponent);
}
