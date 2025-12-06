#include "pch.h"
#include "CBoss2_Start.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CEyeball.h"
#include "CStateMachine.h"

CBoss2_Start::CBoss2_Start(CMonsterBase* owner)
    : m_Owner(owner), m_Eyeball_SpawnCount(0), m_Pattern_Cooldown(0), m_Eyeball_ReachedCount(0)
{

}

CBoss2_Start::~CBoss2_Start()
{
}

HRESULT CBoss2_Start::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 3.0f;

    m_Eyeball_SpawnCount = 0;
    m_Eyeball_ReachedCount = 0;

    return S_OK;
}

void CBoss2_Start::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Start", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
    m_Eyeball_SpawnCount = 0;
    m_Eyeball_ReachedCount = 0;

    m_AlreadyTriggered = false;
}

_int CBoss2_Start::Update_State(const _float& timeDelta)
{
    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
        {
            _int curFrame = animator->Get_CurFrame();

            // 17 , 32 , 50
            if (m_Eyeball_SpawnCount == 0 && curFrame == 17)
            {
                Spawn_Eyeball(1);
                m_Eyeball_SpawnCount = 1;
            }
            else if (m_Eyeball_SpawnCount == 1 && curFrame == 32)
            {
                Spawn_Eyeball(2);
                m_Eyeball_SpawnCount = 2;
            }
            else if (m_Eyeball_SpawnCount == 2 && curFrame == 50)
            {
                Spawn_Eyeball(3);
                m_Eyeball_SpawnCount = 3;
            }

            if (m_Eyeball_ReachedCount >= 3)
            {
                Trigger_Eyeball_Descend(0);
                Trigger_Eyeball_Descend(2);

                m_fCurStateElapsedTime += timeDelta;
            }

            if (m_fCurStateElapsedTime >= 0.5f)
            {
                Trigger_Eyeball_Descend(1);

                m_AlreadyTriggered = true;
            }

            // 애니메이션 끝났으면 다음 상태로
            if (animator->Is_AnimationFinished() && m_AlreadyTriggered)
            {
                if (m_Eyeball_ReachedCount >= 3)
                    return CBossPhase2::PATTERN::PHASE2_IDLE;
            }
        }
    }

    //if (m_Controller)
    //{
    //    auto animator = m_Controller->Get_OwnerAnimator();
    //    if (animator && animator->Is_AnimationFinished())
    //    {
    //        return CBossPhase2::PATTERN::PHASE2_EYEBALL;
    //    }
    //}

    // 세팅한 m_fCurStateElapsedTime 초 뒤 끝
    //if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    //{
    //    return CBossPhase2::PATTERN::PHASE2_EYEBALL;
    //}

    return CBossPhase2::PATTERN::PHASE2_START;
}

void CBoss2_Start::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

void CBoss2_Start::Trigger_Eyeball_Descend(_int index)
{
    if (m_Eyeballs[index] && m_Eyeballs[index]->Get_Controller())
    {
        auto stateMachine = m_Eyeballs[index]->Get_Controller()->Get_StateMachine();
        if (stateMachine)
            stateMachine->Change_State(CEyeball::PATTERN::EYEBALL_START_EXIT);
    }
}

void CBoss2_Start::Spawn_Eyeball(_int index)
{
    auto boss = static_cast<CBossPhase2*>(m_Owner);
    if (!boss || !m_Controller)  
        return;

    auto bossTransform = m_Controller->Get_OwnerTransform();
    if (!bossTransform) 
        return;

    CEyeball* eyeball = CEyeball::Create(boss->Get_GraphicDev(), m_Owner);
    if (!eyeball)  
        return;

    // Eyeball 사망 콜백 -> 보스 Roar 이후 다른 패턴 시작
    eyeball->Set_OnDeathCallback([boss]()
        {
            if (boss && boss->Get_Controller())
            {
                auto stateMachine = boss->Get_Controller()->Get_StateMachine();
                if (stateMachine)
                {
                    // ROAR로 변경하는게 아니라, 다른 공격으로 전환하면서 ROAR는 그냥 그 전에 애니메이션만 출력하도록 하기
                    stateMachine->Change_State(CBossPhase2::PATTERN::PHASE2_ROAR);
                }
            }
        });

    eyeball->Set_OnReachedCallback([this, index]
        {
            m_Eyeball_ReachedCount++;
        });

    _vec3 spawnPos = bossTransform->Get_Pos();

    if (index == 1)
    {
        spawnPos.x += -2.f;
        spawnPos.y += 0.f;
        spawnPos.z -= 3.f;
    }
    else if (index == 2)
    {
        spawnPos.y += 1.5f;
        spawnPos.z -= 3.f;
    }
    else if (index == 3)
    {
        spawnPos.x += 2.f;
        spawnPos.y += 0.f;
        spawnPos.z -= 3.f;
    }

    eyeball->Get_Transform()->Set_Pos(spawnPos);

    ::SpawnObjectOnLayer(eyeball, OBJTYPE::MONSTER);

    eyeball->Initialize_GameObject();

    // 참조 저장
    if (index == 1)
        m_Eyeballs[0] = eyeball;
    else if (index == 2)
        m_Eyeballs[1] = eyeball;
    else if (index == 3)
        m_Eyeballs[2] = eyeball;
}

CBoss2_Start* CBoss2_Start::Create(CMonsterBase* owner)
{
    auto state = new CBoss2_Start(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Idle Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Start::Free()
{
    
}
