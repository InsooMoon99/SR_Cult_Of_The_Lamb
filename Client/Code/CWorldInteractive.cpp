#include "pch.h"
#include "CWorldInteractive.h"
#include "CAnimator.h"
#include "CState.h"
#include "CCombatStat.h"
#include "CBoxCollider.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CCreateHelper.h"
#include "CPlayer.h"

CWorldInteractive::CWorldInteractive(DEVICE graphicDev) : CRenderObject(graphicDev)
{
}

CWorldInteractive::CWorldInteractive(const CWorldInteractive& rhs) : CRenderObject(rhs),m_IsPlayerOverlap(rhs.m_IsPlayerOverlap)
{
}

CWorldInteractive::~CWorldInteractive()
{
}

HRESULT CWorldInteractive::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Hp(100.f);
    m_CombatStatCom->Set_MaxHp(100.f);
    m_CombatStatCom->Set_Attack(50.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    m_TransformCom->Set_Pos(_vec3(20.f, 1.f, 15.f));
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::INTERACTIVEOBJ);


    return S_OK;
}

void CWorldInteractive::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CWorldInteractive::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    // 플레이어가 가까이 있을떄만
    if (m_IsPlayerOverlap == true)
    {
        // 
    }

   
    return exit;
}

void CWorldInteractive::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CWorldInteractive::Render_GameObject()
{
    // Transform 설정
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더 (디버그용)
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

void CWorldInteractive::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if(ply)
    m_IsPlayerOverlap = true;

}

void CWorldInteractive::OnEndOverlap(CCollider* self, CCollider* other)
{
    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if(m_IsPlayerOverlap==true&& ply!=nullptr)
        m_IsPlayerOverlap == false;

}

bool CWorldInteractive::Is_Hovering()
{
    return false;
}

HRESULT CWorldInteractive::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    // State
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    // BoxCollider
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    return S_OK;
}


void CWorldInteractive::Free()
{
    CRenderObject::Free();
}
