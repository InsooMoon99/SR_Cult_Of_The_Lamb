#include "pch.h"
#include "CBossPhase2.h"
#include "CCreateHelper.h"
#include "CAnimator.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CBossController.h"
#include "CBoss2_StateMachine.h"
#include "CCombatStat.h"
#include "CBoxCollider.h"
#include "CUI_BossHp.h"

CBossPhase2::CBossPhase2(DEVICE graphicDev)
    : CMonsterBase(graphicDev), m_BossController(nullptr)
{
}

CBossPhase2::CBossPhase2(const CBossPhase2& rhs)
    : CMonsterBase(rhs), m_BossController(rhs.m_BossController)
{
}

CBossPhase2::~CBossPhase2()
{
}

HRESULT CBossPhase2::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_MaxHp(1000.f);
    m_CombatStatCom->Set_HitState(false); // Hit State로 변경할건지

    m_TransformCom->Set_Scale(_vec3(13.7f, 6.3f, 3.f));

    m_BoxColCom->Set_Size(_vec3(1.2f, 1.6f, 1.f));
    m_BoxColCom->Set_Offset(_vec3(0.f, -2.1f, 0.f));

    return S_OK;
}

void CBossPhase2::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    // State Machine
    auto stateMachine = CBoss2_StateMachine::Create(this);
    m_BossController->Set_StateMachine(stateMachine); 
    Safe_Release(stateMachine);

    
}

_int CBossPhase2::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    // 사망
    if (m_CombatStatCom && m_CombatStatCom->Is_Dead())
    {
        if (m_BossController)
        {
            auto stateMachine = m_BossController->Get_StateMachine();
            if (stateMachine)
            {
                _int curState = stateMachine->Get_CurStateNum();

                if (curState != PATTERN::PHASE2_DEAD)
                {
                    stateMachine->Change_State(PATTERN::PHASE2_DEAD);
                }
            }
        }
    }

    return exit;
}

void CBossPhase2::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

}

void CBossPhase2::Render_GameObject()
{
    CMonsterBase::Render_GameObject();


}

void CBossPhase2::Render_Editor()
{
    CMonsterBase::Render_Editor();

    ImGui::SeparatorText("=== Boss Phase2 ===");

    if (m_BossController && m_BossController->Get_StateMachine())
    {
        auto stateMachine = m_BossController->Get_StateMachine();
        _int curState = stateMachine->Get_CurStateNum();

        ImGui::Text("Current State : %s", Get_Boss2_StateName(curState));

        ImGui::Separator();

        for (int i = 0; i < PATTERN::PHASE2_END; i++)
        {
            if (ImGui::Button(Get_Boss2_StateName(i)))
            {
                stateMachine->Change_State(i);
            }
        }
    }

}

const char* CBossPhase2::Get_Boss2_StateName(_int stateNum)
{
    // 상태 추가될때마다 추가하기
    switch (stateNum)
    {
    case PHASE2_START:      return "PHASE2_Start";
    case PHASE2_IDLE:       return "PHASE2_Idle";
    case PHASE2_ROAR:       return "PHASE2_Roar";
    case PHASE2_EYEBALL:    return "PHASE2_EYEBALL";
    case PHASE2_CHAIN:      return "PHASE2_CHAIN";
    case PHASE2_BULLET:     return "PHASE2_BULLET";
    case PHASE2_DEAD:       return "PHASE2_DEAD";
    case PHASE2_END:        return "PHASE2_END";
    }
    return "UNKNOWN";
}

CBossPhase2* CBossPhase2::Create(DEVICE graphicDev)
{
    CBossPhase2* boss = new CBossPhase2(graphicDev);

    if (FAILED(boss->Ready_GameObject()))
    {
        Safe_Release(boss);
        MSG_BOX("Boss2 Create Failed");
        return nullptr;
    }

    return boss;
}

void CBossPhase2::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Boss2_Start", 68, 0.04f);
    m_AnimatorCom->Create_Animation(L"Boss2_Idle", 68, 0.04f);
    m_AnimatorCom->Create_Animation(L"Boss2_Roar", 69, 0.04f);

    m_AnimatorCom->Create_Animation(L"Boss2_HandSlam", 68, 0.04f);
    m_AnimatorCom->Create_Animation(L"Boss2_Dead", 56, 0.04f);

    // 스프라이트 재생은 상태머신으로
    //m_AnimatorCom->Play_Animation(L"Boss2_Idle", ANIMSTATE::LOOP); 
}

HRESULT CBossPhase2::Add_Component()
{
    if(FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE type;

    type = COMPONENTTYPE::TEX_BOSS2_PHASE2;
    m_TextureCom = CreateProtoComponent<CTexture>(this, type);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ type, m_TextureCom });

    m_AnimatorCom->Set_TextureType(type);

    // Controller
    type = COMPONENTTYPE::BOSS2_CONTROLLER;
    m_BossController = CreateProtoComponent<CBossController>(this, type);
    NULL_CHECK_RETURN(m_BossController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ type, m_BossController });


    return S_OK;
}

void CBossPhase2::Free()
{
    CMonsterBase::Free();
}
