#include "pch.h"
#include "CConstructionMgr.h"

#include "CConstructionPlacer.h"

IMPLEMENT_SINGLETON(CConstructionMgr)

CConstructionMgr::CConstructionMgr()
{ }

CConstructionMgr::~CConstructionMgr()
{
    CConstructionMgr::Free();
}

void CConstructionMgr::Ready_ConstructionMgr(DEVICE graphicDev)
{
    m_GraphicDev = graphicDev;

    m_pPlacer = CConstructionPlacer::Create(m_GraphicDev);
}

void CConstructionMgr::Update_ConstructionMgr(const _float& fTimeDelta)
{
    if (!m_bIsBuilding)
        return;

    if (m_pPlacer)
    {
        m_pPlacer->Update_Placer(fTimeDelta);

        // TODO: 마우스 클릭 체크하여 배치 시도
        // 예: if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
        // {
        //     if (m_pPlacer->Try_PlaceObject()) {
        //         // 배치 성공 시 후처리 (아이템 차감, 건설 모드 종료 or 유지)
        //     }
        // }

        // TODO: 취소 키 체크 (ESC or 우클릭)
        // if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
        //     End_BuildMode();
    }
}

void CConstructionMgr::Render_ConstructionMgr()
{
    if (!m_bIsBuilding)
        return;

    if (m_pPlacer)
        m_pPlacer->Render_Preview();
}

void CConstructionMgr::Start_BuildMode(BUILDING_TYPE type)
{
    if (!m_pPlacer)
        return;

    m_bIsBuilding = true;
    m_pPlacer->Set_PreviewObject(type);
}

void CConstructionMgr::End_BuildMode()
{
    m_bIsBuilding = false;

    // Placer 내부 상태 초기화 필요시 호출
    // m_pPlacer->Reset(); 
}

void CConstructionMgr::Free()
{
    Safe_Release(m_GraphicDev);
    Safe_Release(m_pPlacer);
}
