#include "pch.h"
#include "CRenderer.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CState.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CCombatStat.h"
#include "CMonsterState.h"


CBossPhase1::CBossPhase1(DEVICE graphicDev)
    : CMonsterBase(graphicDev)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::CBossPhase1(const CBossPhase1& rhs)
    : CMonsterBase(rhs)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::~CBossPhase1()
{
}

HRESULT CBossPhase1::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_StateMachine = CBossStateMachine::Create(this);

    m_TransformCom->Set_Scale({ 7,7,1 });
    BaseCenter = { 20 ,m_TransformCom->Get_Scale().y * .5f, 20 };
    m_TransformCom->Set_Pos(StartPos.x, m_TransformCom->Get_Scale().y * .5f, StartPos.z);


    m_CombatStatCom->Set_Hp(200.f);
    m_CombatStatCom->Set_MaxHp(200.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    return S_OK;
}

_int CBossPhase1::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    m_StateMachine->Update_StateMachine(timeDelta);

    return exit;
}

void CBossPhase1::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CBossPhase1::Render_GameObject()
{
    CMonsterBase::Render_GameObject();

    m_StateMachine->Get_CurState()->Render_State();

}

void CBossPhase1::Render_Editor()
{
    ImGui::Text("=== BossP1 ===");

    _int num = m_StateMachine->GetCurStateNum();

    for (int i = 0; i < BOSS_END; i++)
    {
        if (ImGui::Button(GetBossStateName(i)))
        {
            m_StateMachine->Change_State(i);
        }
    }
}

HRESULT CBossPhase1::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BOSSPHASE1 );
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BOSSPHASE1, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BOSSPHASE1);

    return S_OK;
}

void CBossPhase1::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"BossIdle", 100, 0.02f);

    m_AnimatorCom->Create_Animation(L"BossAwake", 105, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossAttack", 97, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossDead", 167, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_up", 50, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_Down", 28, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_start", 115, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_loop", 84, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossGroggy_end", 67, 0.01f);

    m_AnimatorCom->Create_Animation(L"BossChainEff", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"ChainGroundEff",1, 0.02f);

}

const char* CBossPhase1::GetBossStateName(_int StateNum)
{

    switch (StateNum)
    {
    case BOSSAWAKE:         return "BOSSAWAKE";
    case BULLETATTACK:      return "BULLETATTACK";
    case CHAINATTACK:       return "CHAINATTACK";
    case BOSSDEAD:          return "BOSSDEAD";
    case BOSSIDLE:          return "BOSSIDLE";
    case BOSSMOVE:          return "BOSSMOVE";
    case BOSSGROGGRY:       return "BOSSGROGGRY";
    case BOSS_END:          return "BOSS_END";
    }
    return "UNKNOWN";
}

CBossPhase1* CBossPhase1::Create(DEVICE graphicDev)
{
    auto Boss = new CBossPhase1(graphicDev);

    if (FAILED(Boss->Ready_GameObject()))
    {
        MSG_BOX("Boss Create Failed");
        Safe_Release(Boss);
        return nullptr;
    }

    return Boss;
}

void CBossPhase1::Free()
{
    //m_StateMachine->Release();
    Safe_Release(m_StateMachine);

    CMonsterBase::Free();

}
