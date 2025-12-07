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
        if (m_FadeTimer >= 0.1f)                // 잠깐 대기
        {
            m_FadeState = FADE_STATE::SCENE_CHANGE;
        }
        break;

    case FADE_STATE::SCENE_CHANGE:
        Change_SceneMgr(m_NextSceneType);       // 실제 씬 전환
        m_FadeState = FADE_STATE::FADE_IN;
        m_FadeUI->StartFadeIn(m_FadeSpeed);
        break;

    case FADE_STATE::FADE_IN:
        if (m_FadeUI->IsFadeComplete())         // 완료 확인
        {
            m_FadeState = FADE_STATE::NONE;
            m_IsFading = false;
        }
        break;
    }

    fade->Update_GameObject(timeDelta);
}

void CSceneMgr::Render_SceneMgr()
{
    if (!m_IsFading)
        return;

    CUI_Fade* fade = Get_Or_Create_FadeUI();

    if (fade)
    {
        // Z-depth 끄기
        m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

        fade->Render_GameObject();

        // Z-depth 복원
        m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    }
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
    if (m_IsFading)
        return;

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
            m_FadeUI->Ready_GameObject();
			m_FadeUI->AddRef();
        }
    }

    return m_FadeUI;
}
