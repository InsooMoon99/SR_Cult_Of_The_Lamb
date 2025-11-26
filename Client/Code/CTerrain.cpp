#include "pch.h"
#include "CTerrain.h"

#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CTerrainRenderer.h"

CTerrain::CTerrain(DEVICE GraphicDev)
    : CGameObject(GraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    :CGameObject(rhs)
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, -1.f, 0.1f));
    m_TransformCom->Set_Scale(_vec3(1.5f, 1.f, 1.5f)); // y축은 1로 고정해야함


    return S_OK;
}

_int CTerrain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return exit;
}

void CTerrain::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);


}

void CTerrain::Render_GameObject()
{
    if (m_TerrainRendererCom == nullptr)
        return;

    m_TerrainRendererCom->Render_TerrainRenderer();

}

HRESULT CTerrain::Add_Component()
{
    m_BufferCom = CreateProtoComponent<CTerrainTex>(this, COMPONENTTYPE::TERRAIN_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TERRAIN_TEX, m_BufferCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_284);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_284, m_TextureCom });

    m_TerrainRendererCom = CreateProtoComponent<CTerrainRenderer>(this, COMPONENTTYPE::TERRAIN_RENDER);
    NULL_CHECK_RETURN(m_TerrainRendererCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TERRAIN_RENDER, m_TerrainRendererCom });

    FAILED_CHECK_RETURN(m_TerrainRendererCom->Ready_TerrainRenderer(), E_FAIL);

    return S_OK;
}

void CTerrain::Render_Editor()
{
    CGameObject::Render_Editor();

    ImGui::Text("=== Terrain ===");

}

CTerrain* CTerrain::Create(DEVICE GraphicDev)
{
    CTerrain* terrain = new CTerrain(GraphicDev);

    if (FAILED(terrain->Ready_GameObject()))
    {
        Safe_Release(terrain);
        MSG_BOX("Create Tile Failed");
        return nullptr;
    }

    return terrain;
}

void CTerrain::Free()
{
    CGameObject::Free();

}
