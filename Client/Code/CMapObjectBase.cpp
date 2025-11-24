#include "pch.h"
#include "CMapObjectBase.h"

#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CTerrain.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"

CMapObjectBase::CMapObjectBase(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AttachTerrain(false),
      m_Terrain(nullptr)
{ }

CMapObjectBase::CMapObjectBase(const CMapObjectBase& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AttachTerrain(rhs.m_AttachTerrain),
      m_CellX(rhs.m_CellX),
      m_CellZ(rhs.m_CellZ),
      m_Terrain(rhs.m_Terrain)
{ }

CMapObjectBase::~CMapObjectBase()
{ }

HRESULT CMapObjectBase::Ready_GameObject()
{
    return S_OK;
}

_int CMapObjectBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_PRIORITY, this);

    return exit;
}

void CMapObjectBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CMapObjectBase::Render_GameObject()
{
    if (m_TransformCom == nullptr || m_BufferCom == nullptr)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(2);
    m_BufferCom->Render_Buffer();
}

void CMapObjectBase::Set_WorldPos(const _vec3& pos)
{
    if (m_TransformCom)
        m_TransformCom->Set_Pos(pos);
}

HRESULT CMapObjectBase::Add_BaseComponent(COMPONENTTYPE InType)
{
    COMPONENTTYPE type = COMPONENTTYPE::COMPONENT_END;

    // RcTex
    type        = COMPONENTTYPE::RC_TEX;
    m_BufferCom = CreateProtoComponent<CRcTex>(this, type);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ type, m_BufferCom });

    // Transform
    type           = COMPONENTTYPE::TRANSFORM;
    m_TransformCom = CreateProtoComponent<CTransform>(this, type);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ type, m_TransformCom });

    // Texture Component는 자식에서 세팅하기
    m_TextureCom = CreateProtoComponent<CTexture>(this, InType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ InType, m_TextureCom });

    return S_OK;
}

void CMapObjectBase::Free()
{
    CGameObject::Free();
}
