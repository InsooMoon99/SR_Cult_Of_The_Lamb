#include "pch.h"
#include "CConstructionPlacer.h"
#include "CGameObject.h"
#include "CPickingHelper.h"
#include "CTerrain.h"
#include "CTransform.h"

// 임시: 아이템 ID나 프로토타입 키 매핑을 위한 헤더 필요시 추가
// #include "CItemDB.h" 

CConstructionPlacer::CConstructionPlacer(DEVICE graphicDev)
    : m_GraphicDev(graphicDev)
{
    m_GraphicDev->AddRef();
}

CConstructionPlacer::~CConstructionPlacer()
{ }

void CConstructionPlacer::Update_Placer(const _float& fTimeDelta)
{
    NULL_CHECK(m_PreviewObj);

    _vec3 pickPos;

    if (!PickPlaneY(m_GraphicDev, g_hWnd, 0.f, pickPos))
        return;

    // Check Validation
    m_CanPlace = Check_ValidPosition(&pickPos);

    pickPos.y += m_PreviewOffsetY;

    // Update Preview Object Transform
    auto previewTransform = dynamic_cast<CTransform*>(
        m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (previewTransform)
        previewTransform->Set_Pos(pickPos);

    m_PreviewObj->Update_GameObject(fTimeDelta);
}

void CConstructionPlacer::Render_Preview()
{
    NULL_CHECK(m_PreviewObj);

#pragma region --- RenderState & TextureStageStat 변경 ---

    // 알파 블랜딩 활성화
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // 뒤쪽 오브젝트를 가리지 않게 하기 위한 Z-Buffer 기록 끄기
    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    // 색상 혼합
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    // 알파 혼합
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    // 프리뷰 오브젝트 색상 설정
    DWORD previewColor;
    if (m_CanPlace)
        previewColor = D3DCOLOR_ARGB(128, 128, 255, 128);
    else
        previewColor = D3DCOLOR_ARGB(128, 255, 128, 128);

    // 텍스처에 ARGB 색상 적용
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, previewColor);

#pragma endregion

    // 프리뷰 오브젝트 그리기
    m_PreviewObj->Render_GameObject();

#pragma region --- 설정 초기화 ---

    // Z-Write 다시 켜기
    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    // 알파 블렌딩 끄기
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    // 알파 테스트 켜기
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    // Color 연산 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    // Alpha 연산 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    // 팩터 색상 초기화 (흰색)
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

#pragma endregion
}

void CConstructionPlacer::Set_PreviewObject(BUILDING_TYPE type)
{
    if (m_CurBuildingType == type && m_PreviewObj != nullptr)
        return;

    // 기존 미리보기 제거
    Safe_Release(m_PreviewObj);
    m_CurBuildingType = type;

    // TODO: iItemIndex에  해당하는 GameObject 생성
    // 프로토타입 클론 로직 필요. 
    // 예: m_PreviewObj = Prototype_Mgr->Clone_Proto(L"Proto_Building_House");
    // 지금은 껍데기만.
}

_bool CConstructionPlacer::Try_PlaceObject()
{
    if (!m_CanPlace)
        return false;

    // TODO: 실제 오브젝트 생성 요청
    // Logic:
    // 1. 현재 vPickPos 가져오기
    // 2. CStage의 레이어에 실제 오브젝트 Add_Object
    // 3. 인벤토리에서 아이템 차감 (Manager가 호출자이므로 Manager에서 처리하거나 여기서 콜백)

    return true;
}

_bool CConstructionPlacer::Check_ValidPosition(_vec3* pOutPos)
{
    // TODO: 마우스 피킹 구현
    // 일단 기본값 리턴
    if (pOutPos)
        *pOutPos = _vec3(0.f, 0.f, 0.f);

    return true;
}

CConstructionPlacer* CConstructionPlacer::Create(DEVICE graphicDev)
{
    auto constructionPlacer = new CConstructionPlacer(graphicDev);

    NULL_CHECK_RETURN_MSG(constructionPlacer,
                          nullptr,
                          L"ConstructionPlacer Create Failed");

    return constructionPlacer;
}

void CConstructionPlacer::Free()
{
    Safe_Release(m_PreviewObj);
    Safe_Release(m_GraphicDev);
}
