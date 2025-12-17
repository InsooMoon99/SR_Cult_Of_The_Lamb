#include "pch.h"
#include "CMapObjectBase.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTerrain.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CLayer.h"
#include "CPlayer.h"

CMapObjectBase::CMapObjectBase(DEVICE graphicDev)
    : CRenderObject(graphicDev),
      m_TextureCom(nullptr),
      m_AttachTerrain(false), m_IsPreview(false),
      m_Terrain(nullptr)
{
}

CMapObjectBase::CMapObjectBase(const CMapObjectBase& rhs)
    : CRenderObject(rhs),
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
    // 여기에 Light?
    return S_OK;
}

void CMapObjectBase::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

    // 플레이어 찾기
    auto scene = CManagement::GetInstance()->Get_Scene();
    if (scene)
    {
        auto layer = scene->Get_Layer(LAYERTYPE::GAMELOGIC);
        if (layer)
        {
            const auto& objectMap = layer->Get_Objects();
            auto it = objectMap.find(OBJTYPE::PLAYER);

            if (it != objectMap.end() && !it->second.empty())
            {
                auto player = it->second.front();
                m_PlayerTransformCom = player->Get_Transform();
            }
        }
    }
}

_int CMapObjectBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

#pragma region 플레이어 기준으로 Alpha 값 세팅

	/*if (m_PlayerTransformCom && m_TransformCom)
	{
		_vec3 playerPos = m_PlayerTransformCom->Get_Pos();
		_vec3 objPos = m_TransformCom->Get_Pos();

        _float zDist = playerPos.z - objPos.z;

		_bool isBlocking = (zDist > 0.f && zDist < 3.f) &&
                           (abs(objPos.x - playerPos.x) < 0.8f) &&
                           (abs(objPos.y - playerPos.y) < 1.2f);
		
		m_TargetAlpha = isBlocking ? 0.25f : 1.f;
	}

    m_Alpha += (m_TargetAlpha - m_Alpha) * timeDelta * 10.f;*/


#pragma endregion

#pragma region 카메라 기준으로 Alpha값 세팅

    //if (m_PlayerTransformCom && m_TransformCom)
    //{
    //    _vec3 playerPos = m_PlayerTransformCom->Get_Pos();
    //    _vec3 objPos = m_TransformCom->Get_Pos();
    //
    //    // 오브젝트가 카메라 앞에있는지 체크, Z값이 작을수록 카메라에 가까운 거
    //    _bool inFront = (objPos.z < playerPos.z);
    //
    //    _bool xOverlap = (abs(objPos.x - playerPos.x) < 0.8f);
    //    _bool yOverlap = (objPos.y > playerPos.y - 0.5f) &&
    //        (objPos.y < playerPos.y + 2.5f);
    //
    //    _bool isBlocking = inFront && xOverlap && yOverlap;
    //
    //    m_TargetAlpha = isBlocking ? 0.3f : 1.f;  
    //}
    //// 
    //m_Alpha += (m_TargetAlpha - m_Alpha) * timeDelta * 10.f;  

#pragma endregion

    return exit;
}

void CMapObjectBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

    if (!m_IsPreview)
        CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);
}

void CMapObjectBase::Render_GameObject()
{
    if (!m_TransformCom || !m_BufferCom || !m_TextureCom)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    m_TextureCom->Set_Texture(m_TextureIndex); 
    m_BufferCom->Render_Buffer();

    Render_Reset();
}

void CMapObjectBase::Set_Material()
{
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    D3DMATERIAL9 material;

    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse  = COLOR(1.f, 1.f, 1.f, m_Alpha);
    material.Ambient  = COLOR(1.f, 1.f, 1.f, m_Alpha);
    material.Specular = COLOR(1.f, 1.f, 1.f, m_Alpha);
    material.Emissive = COLOR(0.f, 0.f, 0.f, 1.f);
    material.Power = 0.f;

    m_GraphicDev->SetMaterial(&material);
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
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // 자식에서 Add_BaseComponent에 타입 넣어주기
    m_TextureCom = CreateProtoComponent<CTexture>(this, InType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ InType, m_TextureCom });

    return S_OK;
}

void CMapObjectBase::Render_Setting()
{
    CRenderObject::Render_Setting();

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	/*if (m_Alpha < 1.f)
	{
		m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}*/

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB((_int)(m_Alpha * 255), 255, 255, 255));

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
}

void CMapObjectBase::Render_Reset()
{
    // 복원 후 부모호출
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    CRenderObject::Render_Reset();

	//m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CMapObjectBase::Free()
{
    CRenderObject::Free();
}
