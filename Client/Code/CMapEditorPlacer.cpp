#include "pch.h"
#include "CMapEditorPlacer.h"
#include "CMapObject_Helper.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CMapObjectMgr.h"
#include "CPickingHelper.h"
#include "CScene.h"

CMapEditorPlacer::CMapEditorPlacer(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),
      m_PreviewObj(nullptr),
      m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
      m_Placing(false), m_GrassIndex(0), m_RubbleIndex(0), m_DecoIndex(0), m_PreviewScale(1.f)
{
    if (m_GraphicDev)
        m_GraphicDev->AddRef();
}

CMapEditorPlacer::CMapEditorPlacer(const CMapEditorPlacer& rhs)
    : m_GraphicDev(rhs.m_GraphicDev),
      m_PreviewObj(nullptr),
      m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
      m_Placing(false), m_GrassIndex(0), m_RubbleIndex(0), m_DecoIndex(0), m_PreviewScale(1.f)
{
    if (m_GraphicDev)
        m_GraphicDev->AddRef();
}

CMapEditorPlacer::~CMapEditorPlacer()
{ }

void CMapEditorPlacer::Update_MapPlacer(const _float& timeDelta)
{
    if (!m_Placing || m_PreviewObj == nullptr)
        return;

    // 마우스 -> 월드좌표
    _vec3 worldPos;
    if (Get_MouseWorldPos(worldPos))
    {
        m_PreviewObj->Set_WorldPos(worldPos);
    }

    m_PreviewObj->Update_GameObject(timeDelta);
    m_PreviewObj->LateUpdate_GameObject(timeDelta);

    // ImGui 클릭 시에는 화면 클릭 무시
    /*ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;*/

    // 좌클릭 하면 배치하기
    auto input = CDInputMgr::GetInstance();

    _long wheelDelta = input->Get_DIMouseMove(DIMS_Z);
    if (wheelDelta != 0)
    {
        // 양수면 확대, 음수면 축소
        _float scaleChange = (wheelDelta > 0 ? 0.1f : -0.1f);
        m_PreviewScale = max(0.1f, min(5.f, m_PreviewScale));
    }

    if (input->MouseDown(DIM_LB))
    {
        CScene* scene = CManagement::GetInstance()->Get_Scene();
        if (scene)
        {
            CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
            if (layer)
            {
                CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, m_SelectedType);
                if (newObj)
                {
                    newObj->Set_TextureIndex(Get_CurTextureIndex());
                    newObj->Set_WorldPos(_vec3(worldPos.x, worldPos.y + 0.2f, worldPos.z));
                    layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);

                    CMapObjectMgr::GetInstance()->Add_Placement(newObj);
                }
            }
        }
    }

    // 우클릭 : 배치 종료
    if (input->MouseDown(DIM_RB))
    {
        m_Placing = false;
        Safe_Release(m_PreviewObj);
    }
}

void CMapEditorPlacer::Render_Privew()
{
    if (!m_Placing || m_PreviewObj == nullptr)
        return;

    // 알파 테스트 끄기. 꺼줘야 블렌딩과 충돌이 나지 않는다.
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // 알파 블렌딩
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128, 255, 255, 255)); 

    m_PreviewObj->Render_GameObject();

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CMapEditorPlacer::Start_Place(MAPOBJTYPE type)
{
    // 기존 프리뷰 제거
    Cancel_Place();

    m_SelectedType = type;

    // 새 프리뷰 생성
    m_PreviewObj = CreateMapObjectType(m_GraphicDev, type);
    if (m_PreviewObj)
    {
        m_PreviewObj->Set_Preview(true);
        m_PreviewObj->Set_TextureIndex(Get_CurTextureIndex());
        m_Placing = true;
        return;
    }

    m_Placing = false;
}

void CMapEditorPlacer::Cancel_Place()
{
    m_Placing = false;

    Safe_Release(m_PreviewObj);
    m_PreviewObj = nullptr;

    m_SelectedType = MAPOBJTYPE::MAPOBJ_END;
}

bool CMapEditorPlacer::Get_MouseWorldPos(_vec3& outPos)
{
    if (m_GraphicDev == nullptr)
        return false;

    if(!PickPlaneY(m_GraphicDev, g_hWnd, 0.f, outPos))
    {
        return false;
    }

    outPos.y = 0.f;

    return true;
}

_int CMapEditorPlacer::Get_CurTextureIndex()
{
    switch (m_SelectedType)
    {
    case MAPOBJTYPE::GRASS:   return m_GrassIndex;
    case MAPOBJTYPE::RUBBLE:  return m_RubbleIndex;
    case MAPOBJTYPE::DECO:    return m_DecoIndex;
    default:                  return 0;
    }
}

CMapEditorPlacer* CMapEditorPlacer::Create(DEVICE graphicDev)
{
    CMapEditorPlacer* placer = new CMapEditorPlacer(graphicDev);

    if (placer)
        return placer;

    MSG_BOX("Create Failed Edit_Placer");
    return nullptr;
}

void CMapEditorPlacer::Free()
{
    Cancel_Place();

    Safe_Release(m_GraphicDev);
}
