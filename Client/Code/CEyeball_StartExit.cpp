#include "pch.h"
#include "CEyeball_StartExit.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CTransform.h"

CEyeball_StartExit::CEyeball_StartExit(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_IsReachedTarget(false), m_IsAnimFinished(false),
      m_IsFinalPosSet(false)
{
}

CEyeball_StartExit::~CEyeball_StartExit()
{
}

void CEyeball_StartExit::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Exit", ANIMSTATE::ONCE);
    }

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
		m_StartPos = transform->Get_Pos();

		m_StartTargetPos = m_StartPos;
		m_StartTargetPos.y = FLOOR_POS1.y;
    }

    m_fCurStateElapsedTime = 0.f;

    m_IsReachedTarget = false;
    m_IsAnimFinished = false;
    m_IsFinalPosSet = false;
}

_int CEyeball_StartExit::Update_State(const _float& timeDelta)
{

    m_fCurStateElapsedTime += timeDelta;

	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        if (!m_IsReachedTarget)
        {
			_vec3 currentPos = transform->Get_Pos();

			_vec3 downMovePos;
			D3DXVec3Lerp(&downMovePos, &currentPos, &m_StartTargetPos, m_LerpSpeed);

			transform->Set_Pos(downMovePos);

			// 목표 거리 도달
			_vec3 dir = m_StartTargetPos - downMovePos;
			float length = D3DXVec3Length(&dir);
			if (length < 0.1f)
			{
				transform->Set_Pos(m_StartTargetPos);
				m_IsReachedTarget = true;
			}
        }
        else if (m_IsReachedTarget && ! m_IsAnimFinished)
        {
            CAnimator* animator = m_Controller->Get_OwnerAnimator();
            if (animator)
            {
                bool isAnimFinished = animator->Is_AnimationFinished();

                if (isAnimFinished)
                {
                    _vec3 finalPos = _vec3(
                        transform->Get_Pos().x,
                        FLOOR_POS2.y,
                        transform->Get_Pos().z
                    );

                    transform->Set_Pos(finalPos);

                    m_IsFinalPosSet = true;
                }
            }
        }
    }

	// 목표 도달 후 일정 시간 대기 후 GROUND으로 전환 후 이동
    if (m_IsFinalPosSet && m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_START_GROUND;
    }

    return CEyeball::PATTERN::EYEBALL_START_EXIT;  
}

void CEyeball_StartExit::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_StartExit::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

	m_LerpSpeed = 0.05f;
    m_IsReachedTarget = false;
    m_IsAnimFinished = false;
    m_IsFinalPosSet = false;        


    return S_OK;
}

CEyeball_StartExit* CEyeball_StartExit::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_StartExit(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_StartExit::Free()
{
    
}
