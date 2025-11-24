#include "pch.h"
#include "CMonsterState.h"

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
    m_fCurStateElapsedTime += timeDelta;

    return 0;
}

void CMonsterState::Find_Player()
{
}
