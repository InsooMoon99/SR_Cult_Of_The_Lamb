#include "pch.h"
#include "CFireball.h"
#include "CProjectile.h"
#include "CCreateHelper.h"
#include "CGameObject.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"

CFireball::CFireball(DEVICE graphicDev)
    : CGameObject(graphicDev),
    m_ProjectileCom(nullptr),
    m_TransformCom(nullptr),
    m_Owner(nullptr)
{

}

CFireball::CFireball(const CFireball& rhs)
    : CGameObject(rhs),
    m_ProjectileCom(nullptr),   
    m_TransformCom(nullptr),
    m_Owner(nullptr)
{

}

CFireball::~CFireball()
{

}

HRESULT CFireball::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CFireball::Update_GameObject(const _float& timeDelta)
{
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA,this);
        return CGameObject::Update_GameObject(timeDelta);
}

CFireball* CFireball::Create(DEVICE graphicDev)
{
    auto fireball = new CFireball(graphicDev);

    if (FAILED(fireball->Ready_GameObject()))
    {
        Safe_Release(fireball);
        MSG_BOX("Fireball Create Failed");
        return nullptr;
    }
    return fireball;
}

void CFireball::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();
}

void CFireball::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CFireball::Render_GameObject()
{
    m_TextureCom->Set_Texture();
    m_BufferCom->Render_Buffer();
}

void CFireball::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CFireball::OnStayOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnStayOverlap(self, other);
}

void CFireball::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CFireball::Render_Editor()
{
    CGameObject::Render_Editor();
}


HRESULT CFireball::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BULLET);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BULLET, m_TextureCom });

    // Projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });

    return S_OK;
}

void CFireball::Free()
{
    CGameObject::Free();
}
