#include "pch.h"
#include "CMainApp.h"

#include "CCollisionManager.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CEditContext.h"
#include "CFontMgr.h"
#include "CFrameMgr.h"
#include "CGraphicDev.h"
#include "CImGuiManager.h"
#include "CItemDB.h"
#include "CLightMgr.h"
#include "CLogo.h"
#include "CMainEditorMgr.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSceneMgr.h"
#include "CTimerMgr.h"

CMainApp::CMainApp()
    : m_ManagementClass(CManagement::GetInstance()),
      m_DeviceClass(nullptr), m_GraphicDev(nullptr)
{ }

CMainApp::~CMainApp()
{ }

HRESULT CMainApp::Ready_MainApp()
{
    if (FAILED(Ready_DefaultSetting(&m_GraphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Scene(m_GraphicDev)))
        return E_FAIL;

    // ImGui
    {
        CImGuiManager::GetInstance()->InitImGui(g_hWnd, m_GraphicDev);

        CEditContext* ctx = CEditContext::Create();
        CMainEditorMgr::GetInstance()->Set_EditContext(ctx);
    }
    

#pragma region 데이터 파싱 테스트

    if (FAILED(CItemDB::GetInstance()->LoadFromJson("ItemData.txt")))
        return E_FAIL;

#pragma endregion

    return S_OK;
}

int CMainApp::Update_MainApp(const float& timeDelta)
{
    m_ManagementClass->Commit_ChangeScene();

    CDInputMgr::GetInstance()->Update_InputDev();

    m_ManagementClass->Update_Scene(timeDelta);

    CCollisionManager::GetInstance()->Update();

    // Imgui
    {
        CImGuiManager::GetInstance()->Update();
        /*ImGui::Begin("New Windows");
        ImGui::End();*/
    }

    return 0;
}

void CMainApp::LateUpdate_MainApp(const float& timeDelta)
{
    m_ManagementClass->LateUpdate_Scene(timeDelta);
}

void CMainApp::Render_MainApp()
{
    m_DeviceClass->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

    m_ManagementClass->Render_Scene(m_GraphicDev);

    CMainEditorMgr::GetInstance()->Render();
    CImGuiManager::GetInstance()->Render();

    m_DeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(DEVICE* graphicDev)
{
    if (FAILED(CGraphicDev::GetInstance()
        ->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_DeviceClass)))
        return E_FAIL;

    m_DeviceClass->AddRef();

    (*graphicDev) = m_DeviceClass->Get_GraphicDev();

    (*graphicDev)->AddRef();

    // 조명 연산 끄기 옵션. 나중에는 조명을 끄는 행위는 하지 말자 (온오프 자주 하면 부하↑)
    (*graphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);
    // D3DRS_ZENABLE : Z버퍼에 Z값을 기록, Z 정렬 수행 여부를 묻는 옵션 : On
    (*graphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);
    // D3DRS_ZWRITEENABLE : Z버퍼에 Z값을 기록할 지 말 지 여부를 묻는 옵션 : On
    (*graphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    // 텍스처가 너무 깨지지 않도록 선형 보간 필터링 적용
    (*graphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    (*graphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // Fonts
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"견명조", L"견명조", 20, 20, FW_HEAVY)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"궁서", L"궁서", 20, 15, FW_THIN)))
        return E_FAIL;

    // DInput
    if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Scene(DEVICE graphicDev)
{
   /* Engine::CScene* logo = CLogo::Create(graphicDev);

    if (nullptr == logo)
        return E_FAIL;

    if (FAILED(m_ManagementClass->Set_Scene(logo)))
    {
        MSG_BOX("Logo Setting Failed");
        return E_FAIL;
    }

    return S_OK;*/

    return CSceneMgr::GetInstance()->Ready_SceneMgr(graphicDev);
}

CMainApp* CMainApp::Create()
{
    auto mainApp = new CMainApp;

    if (FAILED(mainApp->Ready_MainApp()))
    {
        MSG_BOX("CMainApp Create Failed");
        Safe_Release(mainApp);
        return nullptr;
    }

    return mainApp;
}

void CMainApp::Free()
{
    Safe_Release(m_GraphicDev);
    Safe_Release(m_DeviceClass);

    CCollisionManager::DestroyInstance();
    CLightMgr::DestroyInstance();
    CDInputMgr::DestroyInstance();
    CFontMgr::DestroyInstance();
    CRenderer::DestroyInstance();
    CProtoMgr::DestroyInstance();
    CFrameMgr::DestroyInstance();
    CTimerMgr::DestroyInstance();
    CMainEditorMgr::DestroyInstance();
    CImGuiManager::DestroyInstance();
    CManagement::DestroyInstance();
    CGraphicDev::DestroyInstance();
    CSceneMgr::DestroyInstance();

#pragma region 데이터 파싱 테스트

    CItemDB::DestroyInstance();
    CDropSystem::DestroyInstance();

#pragma endregion
}
