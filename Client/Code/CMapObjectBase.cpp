#include "pch.h"
#include "CMapObjectBase.h"

#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTerrain.h"
#include "CTexture.h"
#include "CTransform.h"

CMapObjectBase::CMapObjectBase(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AttachTerrain(false), m_IsPreview(false),
      m_Terrain(nullptr)
{
}

CMapObjectBase::CMapObjectBase(const CMapObjectBase& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AttachTerrain(rhs.m_AttachTerrain), m_IsPreview(false),
      m_CellX(rhs.m_CellX),
      m_CellZ(rhs.m_CellZ),
      m_Terrain(rhs.m_Terrain)
{
}

CMapObjectBase::~CMapObjectBase()
{ }

HRESULT CMapObjectBase::Ready_GameObject()
{
    return S_OK;
}

_int CMapObjectBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    if (!m_IsPreview)
        CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_PRIORITY_ALPHA, this);

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

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // 알파 테스트 
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    m_TextureCom->Set_Texture(m_TextureIndex);
    m_BufferCom->Render_Buffer();

    //m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMapObjectBase::Set_WorldPos(const _vec3& pos)
{
    if (m_TransformCom == nullptr)
    {
        CComponent* com = Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM);
        m_TransformCom = dynamic_cast<CTransform*>(com);

        if (m_TransformCom == nullptr)
            return; 
    }

    m_TransformCom->Set_Pos(pos);
}

HRESULT CMapObjectBase::Add_BaseComponent(COMPONENTTYPE InType)
{
    // RcTex
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // 자식에서 Add_BaseComponent에 타입 넣어주기
    m_TextureCom = CreateProtoComponent<CTexture>(this, InType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ InType, m_TextureCom });

    return S_OK;
}

void CMapObjectBase::Free()
{
    CGameObject::Free();
}
