#include "pch.h"
#include "CEyeball_Ground.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CRandomMgr.h"
#include "CTransform.h"

CEyeball_Ground::CEyeball_Ground(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_MaxMoveDistance(0)
{
}

CEyeball_Ground::~CEyeball_Ground()
{
}

void CEyeball_Ground::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Start", ANIMSTATE::LOOP);
    }

    // 플레이어 위치 찾기
    Find_Player();

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        m_StartPos = transform->Get_Pos();

        _vec3 playerPos = m_pPlayerTransformCom->Get_Pos();
        _vec3 eyeballPos = transform->Get_Pos();

        m_TargetDir = playerPos - eyeballPos;

        m_TargetDir.y = 0.f;

        D3DXVec3Normalize(&m_TargetDir, &m_TargetDir);

        m_TargetPos = _vec3(playerPos.x, 0.001f, playerPos.z);
    }

    m_fCurStateElapsedTime = 0.f;
}

_int CEyeball_Ground::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_ENTER;
    }

    if (transform && m_pPlayerTransformCom)
    {
        _vec3 playerPos = m_pPlayerTransformCom->Get_Pos();
        _vec3 eyeballPos = transform->Get_Pos();

        _vec3 moveOffset = eyeballPos - m_StartPos;
        float moveDistance = D3DXVec3Length(&moveOffset);

        if (moveDistance >= m_MaxMoveDistance)
        {
            return CEyeball::PATTERN::EYEBALL_GROUND;
        }

        const _float moveSpeed = 8.f;
        transform->Move_Pos(m_TargetDir, timeDelta, moveSpeed);
    }

    return CEyeball::PATTERN::EYEBALL_GROUND;
}

void CEyeball_Ground::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Ground::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 2.5f;

    m_StartPos = _vec3(0.f, 0.f, 0.f);
    m_TargetDir = _vec3(0.f, 0.f, 0.f);
    m_TargetPos = _vec3(0.f, 0.f, 0.f);

    _int random = CRandomMgr::GetInstance()->Get_Int_Random(-4, 8);

    m_MaxMoveDistance = 10.f + static_cast<_float>(random);

    return S_OK;
}

CEyeball_Ground* CEyeball_Ground::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Ground(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Ground::Free()
{
    
}
