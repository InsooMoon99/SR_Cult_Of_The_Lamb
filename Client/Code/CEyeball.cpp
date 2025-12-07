#include "pch.h"
#include "CEyeball.h"
#include "CCombatStat.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CEyeballController.h"
#include "CEyeball_StateMachine.h"
#include "CMonsterController.h"

CEyeball::CEyeball(DEVICE graphicDev, CMonsterBase* boss)
    : CMonsterBase(graphicDev), m_EyeballController(nullptr), m_Boss(boss)
{
}

CEyeball::CEyeball(const CEyeball& rhs, CMonsterBase* boss)
    : CMonsterBase(rhs), m_EyeballController(nullptr), m_Boss(rhs.m_Boss)
{
}

CEyeball::~CEyeball()
{

}

HRESULT CEyeball::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_MaxHp(20.f);
    m_CombatStatCom->Set_Hp(20.f);

    return S_OK;
}

void CEyeball::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    // State Machine
    
    auto stateMachine = CEyeball_StateMachine::Create(this);
    m_EyeballController->Set_StateMachine(stateMachine);
    Safe_Release(stateMachine);

}

_int CEyeball::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (m_CombatStatCom && m_CombatStatCom->Is_Dead())
    {
        if (m_EyeballController)
        {
            auto stateMachine = m_EyeballController->Get_StateMachine();
            if (stateMachine)
            {
                _int curState = stateMachine->Get_CurStateNum();

                if (curState != PATTERN::EYEBALL_DEAD)
                {
                    stateMachine->Change_State(PATTERN::EYEBALL_DEAD);
                }
            }
        }
    }


    return exit;
}

void CEyeball::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

}

void CEyeball::Render_GameObject()
{
    CMonsterBase::Render_GameObject();

}

void CEyeball::Render_Editor()
{
    CMonsterBase::Render_Editor();

    ImGui::SeparatorText("=== Eyeball ===");

    if (m_EyeballController && m_EyeballController->Get_StateMachine())
    {
        auto stateMachine = m_EyeballController->Get_StateMachine();
        _int curState = stateMachine->Get_CurStateNum();

        ImGui::Text("Current State : %s", Get_StateName(curState));

        ImGui::Separator();

        for (int i = 0; i < PATTERN::EYEBALL_END; i++)
        {
            if (ImGui::Button(Get_StateName(i)))
            {
                stateMachine->Change_State(i);
            }
        }
    }

}

void CEyeball::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

}

void CEyeball::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

void CEyeball::Set_OnDeathCallback(OnEyeDeadCallback callback)
{
    m_OnEyeDeadCallback = callback;
}

void CEyeball::Trigger_OnDeath()
{
    if (m_OnEyeDeadCallback)
        m_OnEyeDeadCallback();
}

void CEyeball::Set_OnReachedCallback(OnReachedTargetCallback callback)
{
    m_OnReachedTarget_Callback = callback;
}

void CEyeball::Trigger_OnReachedTarget()
{
    if (m_OnReachedTarget_Callback)
        m_OnReachedTarget_Callback();

}

void CEyeball::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Eyeball_Start", 59, 0.04f, _vec2(0.8f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eyeball_Exit", 34, 0.04f, _vec2(0.8f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eyeball_Enter", 49, 0.04f, _vec2(0.8f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eyeball_Die", 25, 0.04f, _vec2(0.8f, 1.f));

    m_AnimatorCom->Create_Animation(L"Eyeball_Anti", 44, 0.04f, _vec2(0.8f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eyeball_Attack_Loop", 45, 0.04f, _vec2(0.8f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eyeball_Attack_End", 54, 0.04f, _vec2(0.8f, 1.f));
}

HRESULT CEyeball::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE type;

    type = COMPONENTTYPE::TEX_BOSS2_EYEBALL;
    m_TextureCom = CreateProtoComponent<CTexture>(this, type);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ type, m_TextureCom });

    m_AnimatorCom->Set_TextureType(type);

    // Controller
    type = COMPONENTTYPE::EYEBALL_CONTROLLER;
    m_EyeballController = CreateProtoComponent<CEyeballController>(this, type);
    NULL_CHECK_RETURN(m_EyeballController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ type, m_EyeballController });

    return S_OK;
}

const char* CEyeball::Get_StateName(_int stateNum)
{
    // 상태 추가될때마다 추가하기
    switch (stateNum)
    {
    case EYEBALL_START:             return "EYEBALL_START";
    case EYEBALL_START_EXIT:        return "EYEBALL_START_EXIT";
    case EYEBALL_IDLE:              return "EYEBALL_IDLE";
    case EYEBALL_EXIT:              return "EYEBALL_EXIT";
    case EYEBALL_ENTER:             return "EYEBALL_ENTER";
    case EYEBALL_START_GROUND:            return "EYEBALL_GROUND";
    case EYEBALL_MOVE:              return "EYEBALL_MOVE";
    case EYEBALL_ATTACK:            return "EYEBALL_ATTACK";
    case EYEBALL_DEAD:              return "EYEBALL_DEAD";
    }
    return "UNKNOWN";
}

CEyeball* CEyeball::Create(DEVICE graphicDev, CMonsterBase* boss)
{
    CEyeball* eyeball = new CEyeball(graphicDev, boss);

    if (FAILED(eyeball->Ready_GameObject()))
    {
        Safe_Release(eyeball);
        MSG_BOX("Eyeball Create Failed");
        return nullptr;
    }

    return eyeball;
}

void CEyeball::Free()
{
    CMonsterBase::Free();

}
