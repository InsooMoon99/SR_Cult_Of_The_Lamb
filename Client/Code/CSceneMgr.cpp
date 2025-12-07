#include "pch.h"
#include "CSceneMgr.h"
#include "CEdit.h"
#include "CLogo.h"
#include "CLogo_Boss.h"
#include "CLogo_Boss2.h"
#include "CMainMenu.h"
#include "CManagement.h"
#include "CStage.h"
#include "Engine_Enum.h"
#include "CStage_Boss1.h"
#include "CStage_Boss2.h"
#include "CStage_Dungeon.h"
#include "CStage_TarotTest.h"
#include "CUI_Fade.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
    : m_pGraphicDev(nullptr),
    m_SceneType(SCENETYPE::SC_END),
    m_FadeUI(nullptr),
    m_IsFading(false),
    m_FadeTimer(0.0f),
    m_NextSceneType(SCENETYPE::SC_END),
    m_FadeSpeed(2.0f),
    m_FadeState(FADE_STATE::NONE)
{

}

CSceneMgr::~CSceneMgr()
{
	CSceneMgr::Free();
}

HRESULT CSceneMgr::Ready_SceneMgr(DEVICE pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

    m_FadeUI = nullptr;

	Engine::CScene* logo = CLogo::Create(pGraphicDev);

	if (nullptr == logo)
		return E_FAIL;

	if (FAILED(CManagement::GetInstance()->Set_Scene(logo)))
	{
		MSG_BOX("Logo Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CSceneMgr::Update_SceneMgr(const _float& timeDelta)
{
    if (!m_IsFading)
        return;

    CUI_Fade* fade = Get_Or_Create_FadeUI();
    if (!fade)
        return;  // 아직 생성 안됐으면 스킵

    m_FadeTimer += timeDelta;

    fade->Update_GameObject(timeDelta);

    switch (m_FadeState)
    {
    case FADE_STATE::FADE_OUT:
        if (m_FadeUI->IsFadeComplete())
        {
            m_FadeState = FADE_STATE::WAITING;
            m_FadeTimer = 0.0f;
        }
        break;

    case FADE_STATE::WAITING:
        if (m_FadeTimer >= 0.3f)                // 잠깐 대기
        {
            m_FadeState = FADE_STATE::SCENE_CHANGE;
        }
        break;

    case FADE_STATE::SCENE_CHANGE:
        Change_SceneMgr(m_NextSceneType);
        m_FadeState = FADE_STATE::WAIT_SCENE_LOAD;
        m_FadeTimer = 0.0f;
        break;

    case FADE_STATE::WAIT_SCENE_LOAD:
        if (m_FadeTimer >= 0.5f)
        {
            m_FadeState = FADE_STATE::FADE_IN;
            m_FadeUI->StartFadeIn(m_FadeSpeed);
        }
        break;
    case FADE_STATE::FADE_IN:
        if (m_FadeUI->IsFadeComplete())
        {
            m_FadeState = FADE_STATE::NONE;
            m_IsFading = false;
        }
        break;
    }

}

void CSceneMgr::Render_SceneMgr()
{
    CUI_Fade* fade = Get_Or_Create_FadeUI();

    if (!fade)
        return;

    // m_IsFading 대신 Alpha 값으로 판단!
    // Alpha가 거의 0이고, fading 중이 아니면 스킵
    if (fade->GetAlpha() <= 0.001f && !m_IsFading)
        return;
    // View/Projection identity 설정...
    D3DXMATRIX matOldView, matOldProj;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &identity);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &identity);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    fade->Render_GameObject();
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

void CSceneMgr::Change_SceneMgr(SCENETYPE typeInput)
{
	_bool     bFailed = false;

	m_SceneType = typeInput;

	switch (m_SceneType)
	{
	case SCENETYPE::SC_MAINMENU:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CMainMenu::Create(m_pGraphicDev))))
			bFailed = true;
		break;
	case SCENETYPE::SC_STAGE:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage::Create(m_pGraphicDev))))
			bFailed = true;
		break;
	case SCENETYPE::SC_EDIT:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CEdit::Create(m_pGraphicDev))))
			bFailed = true;
		break;

    case SCENETYPE::SC_DUNGEON:
        if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage_Dungeon::Create(m_pGraphicDev))))
            bFailed = true;
        break;

	case SCENETYPE::SC_TAROT_TEST:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage_TarotTest::Create(m_pGraphicDev))))
			bFailed = true;
		break;

	case SCENETYPE::SC_BOSS_STAGE1:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage_Boss1::Create(m_pGraphicDev))))
			bFailed = true;
		break;

	case SCENETYPE::SC_BOSS_STAGE2:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage_Boss2::Create(m_pGraphicDev))))
			bFailed = true;
		break;

    case SCENETYPE::SC_LOGO_BOSS1:
        if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss::Create(m_pGraphicDev))))
            bFailed = true;
        break;

	case SCENETYPE::SC_LOGO_BOSS2:
		if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss2::Create(m_pGraphicDev))))
			bFailed = true;
		break;

    default:
		break;
	}
}

void CSceneMgr::Change_SceneMgr_WithFade(SCENETYPE typeInput, _float fadeSpeed)
{
    // 이미 페이딩 중이면 무시
    if (m_IsFading)
        return;

    // 상태가 NONE이 아니어도 무시 (m_IsFading만 리셋된 경우 대비)
    if (m_FadeState != FADE_STATE::NONE)
        return;
    // 2초 이내 같은 씬 요청 무시 (쿨타임)
    static SCENETYPE lastRequestedScene = SCENETYPE::SC_END;
    static DWORD lastRequestTime = 0;

    if (typeInput == lastRequestedScene && (GetTickCount() - lastRequestTime) < 2000)
        return;

    lastRequestedScene = typeInput;
    lastRequestTime = GetTickCount();
    // 나머지 기존 코드...
    CUI_Fade* fade = Get_Or_Create_FadeUI();
    if (!fade)
    {
        Change_SceneMgr(typeInput);
        return;
    }
    m_NextSceneType = typeInput;
    m_FadeSpeed = fadeSpeed;
    m_IsFading = true;
    m_FadeTimer = 0.0f;
    m_FadeState = FADE_STATE::FADE_OUT;
    fade->StartFadeOut(fadeSpeed);
}

void CSceneMgr::Free()
{
    Safe_Release(m_FadeUI);
	Safe_Release(m_pGraphicDev);
}

CUI_Fade* CSceneMgr::Get_Or_Create_FadeUI()
{
    if (m_FadeUI == nullptr)
    {
        m_FadeUI = CUI_Fade::Create(m_pGraphicDev);

        if (m_FadeUI)
        {
            m_FadeUI->Set_UseRenderer(false); 
        }
    }

    return m_FadeUI;
}
