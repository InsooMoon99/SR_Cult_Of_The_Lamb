#include "pch.h"
#include "CMapEditorPlacer.h"

#include "CClient_CreateHelper.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CPickingHelper.h"
#include "CScene.h"

CMapEditorPlacer::CMapEditorPlacer(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),
    m_PreviewObj(nullptr),
    m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
    m_Placing(false)
{
    if (m_GraphicDev)
        m_GraphicDev->AddRef();
}

CMapEditorPlacer::CMapEditorPlacer(const CMapEditorPlacer& rhs)
    : m_GraphicDev(rhs.m_GraphicDev),
    m_PreviewObj(nullptr),
    m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
    m_Placing(false)
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

    // 좌클릭 하면 배치하기
    auto input = CDInputMgr::GetInstance();

    if (input->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
    {
        CScene* scene = CManagement::GetInstance()->Get_Scene();
        if (scene)
        {
            CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
            if (layer)
            {
                CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, MAPOBJTYPE::GRASS);
                if (newObj)
                {
                    newObj->Set_WorldPos(worldPos);
                    layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);
                }
            }
        }
    }

    // 우클릭 : 배치 종료
    if (input->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB) & 0x80)
    {
        m_Placing = false;
        Safe_Release(m_PreviewObj);
    }
}

void CMapEditorPlacer::Render_Privew()
{
    if (!m_Placing || m_PreviewObj == nullptr)
        return;

    m_PreviewObj->Render_GameObject();
}

void CMapEditorPlacer::Start_Place(MAPOBJTYPE type)
{
    m_SelectedType = type;

    // 기존 프리뷰 제거
    Cancel_Place();

    // 새 프리뷰 생성
    m_PreviewObj = CreateMapObjectType(m_GraphicDev, type);
    if (m_PreviewObj)
    {
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
