#include "pch.h"
#include "CEyeball_Start.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CTransform.h"

CEyeball_Start::CEyeball_Start(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_IsReachedTarget(false)
{
}

CEyeball_Start::~CEyeball_Start()
{
}

void CEyeball_Start::Enter()
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

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
		m_StartPos = transform->Get_Pos();
		m_StartTargetPos = m_StartPos;
		m_StartTargetPos.z = m_StartPos.z - 4.f;
    }

    m_fCurStateElapsedTime = 0.f;
	m_IsReachedTarget = false;
}

_int CEyeball_Start::Update_State(const _float& timeDelta)
{
	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        if (!m_IsReachedTarget)
        {
			_vec3 currentPos = transform->Get_Pos();

			_vec3 fowardMovePos;
			D3DXVec3Lerp(&fowardMovePos, &currentPos, &m_StartTargetPos, m_LerpSpeed);

			transform->Set_Pos(fowardMovePos);

            _vec3 dir = m_StartTargetPos - fowardMovePos;
            float length = D3DXVec3Length(&dir);
            if (length < 0.1f) // 목표 거리 도달
            {
                transform->Set_Pos(m_StartTargetPos);

                if (!m_IsReachedTarget)
                {
                    m_IsReachedTarget = true;

                    auto eyeball = static_cast<CEyeball*>(m_Owner);
                    if (eyeball)  
                    {
                        eyeball->Trigger_OnReachedTarget(); 
                    }
                }
            }
        }
    }

	// 목표 도달 후 일정 시간 대기 후 EXIT으로 전환
	if (m_IsReachedTarget)
	{
		m_fCurStateElapsedTime += timeDelta;

        /*if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
        {
			return CEyeball::PATTERN::EYEBALL_START_EXIT;
        }*/
	}

    return CEyeball::PATTERN::EYEBALL_START;
}

void CEyeball_Start::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Start::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

	m_LerpSpeed = 0.08f;
	m_IsReachedTarget = false;

    return S_OK;
}

CEyeball_Start* CEyeball_Start::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Start(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Start::Free()
{
    
}
