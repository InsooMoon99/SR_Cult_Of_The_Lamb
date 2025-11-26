#include "pch.h"
#include "CHumanMonster.h"

#include <CCombatStat.h>

#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CEnumHelper.h"
#include "CManagement.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"

CHumanMonster::CHumanMonster(DEVICE graphicDev)
    : CGameObject(graphicDev), m_BufferCom(nullptr), m_TransformCom(nullptr),
      m_BoxColCom(nullptr),
      m_StateCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr)
{}

CHumanMonster::CHumanMonster(const CHumanMonster& rhs)
    : CGameObject(rhs), m_BufferCom(nullptr), m_TransformCom(nullptr),
      m_BoxColCom(nullptr),
      m_StateCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr)
{}

CHumanMonster::~CHumanMonster()
{}

HRESULT CHumanMonster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_StateCom->Change_State(ACTORSTATE::IDLE);
    m_StateCom->Change_Dir(ACTORDIR::LEFT);

    Animation_Setting();

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(1.2f, 1.2f, 1.2f));

    m_CombatStatCom->Set_MaxHp(50.f);
    m_CombatStatCom->Set_Hp(50);

    return S_OK;
}

_int CHumanMonster::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return exit;
}

void CHumanMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);

    auto playerTransformCom = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
                                                  LAYERTYPE::GAMELOGIC,
                                                  OBJTYPE::PLAYER,
                                                  COMPONENTTYPE::TRANSFORM));

    NULL_CHECK(playerTransformCom);

    _vec3 vPlayerPos{};
    playerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3 vMonsterPos = m_TransformCom->Get_Pos();

    _vec3 vdistance = vMonsterPos - vPlayerPos;

    float distance = D3DXVec3Length(&vdistance);

    if (distance < 10.f)
        m_TransformCom->Chase_Target(&vPlayerPos, timeDelta, 1.f);
}

void CHumanMonster::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());


    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();

    Render_Reset();
    m_BoxColCom->Render();
}

void CHumanMonster::Render_Setting()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
}

void CHumanMonster::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CHumanMonster::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"HumanMonsterIdle", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"HumanMonsterRun", 18, 0.02f);
    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"HumanMonsterIdle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"HumanMonsterRun", ANIMSTATE::LOOP);
}

void CHumanMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);

}

void CHumanMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);

}

HRESULT CHumanMonster::Add_Component()
{
    //buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    //transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_HUMAN_MONSTER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_HUMAN_MONSTER, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_HUMAN_MONSTER);

    // RectCol Component
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // State
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    return S_OK;
}

CHumanMonster* CHumanMonster::Create(DEVICE graphicDev)
{
    auto humanmonster = new CHumanMonster(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("HumanMonster Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CHumanMonster::Free()
{
    CGameObject::Free();
}
