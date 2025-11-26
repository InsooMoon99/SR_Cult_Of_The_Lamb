#include "pch.h"
#include "CRenderer.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CCombatStat.h"
#include "CMonsterState.h"


CBossPhase1::CBossPhase1(DEVICE graphicDev) : CGameObject(graphicDev)
{
}

CBossPhase1::CBossPhase1(const CBossPhase1& rhs) : CGameObject(rhs)
{
}

CBossPhase1::~CBossPhase1()
{
}

HRESULT CBossPhase1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Animation_Setting();

    m_StateMachine = CBossStateMachine::Create(this);

    m_TransformCom->Set_Scale({ 10,10,1 });

    return S_OK;
}

_int CBossPhase1::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    m_StateMachine->Update_StateMachine(timeDelta);

    return exit;
}

void CBossPhase1::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CBossPhase1::Render_GameObject()
{

    m_StateMachine->Get_CurState()->Render_State();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    //
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    //
    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();
    //

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //
    m_BoxColCom->Render(); // Render Reset 이후 호출해야함
}

HRESULT CBossPhase1::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEXTURE, L"BossPhase1Tex");
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEXTURE, m_TextureCom });

    // Animator (순서조심 텍스쳐, 버퍼 이후)
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Ready_Animator();

    // CBoxCollider Componet
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    //m_BoxColCom->Set_Scale(_vec3(2.f, 2.f, 2.f));

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // Stat
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });


    return S_OK;
}

void CBossPhase1::Animation_Setting()
{

    m_AnimatorCom->Create_Animation(L"BossIdle", 100, 0.03f);

    m_AnimatorCom->Create_Animation(L"BossAwake", 105, 0.03f);
    m_AnimatorCom->Create_Animation(L"BossAttack", 97, 0.03f);
    m_AnimatorCom->Create_Animation(L"BossDead", 167, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_up", 50, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_Down", 28, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_start", 115, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_loop", 84, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossGroggy_end", 67, 0.02f);

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
    CGameObject::Free();

}
