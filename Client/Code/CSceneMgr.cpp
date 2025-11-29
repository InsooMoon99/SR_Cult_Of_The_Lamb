#include "pch.h"
#include "CSceneMgr.h"

#include "CEdit.h"
#include "CLogo.h"
#include "CMainMenu.h"
#include "CManagement.h"
#include "CStage.h"
#include "Engine_Enum.h"
#include "CStage_Boss1.h"
#include "CStage_Boss2.h"
#include "CStage_TaroTest.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
{
    m_pGraphicDev = nullptr;
    m_SceneType = SCENETYPE::SC_END;
}

CSceneMgr::~CSceneMgr()
{
    CSceneMgr::Free();
}

HRESULT CSceneMgr::Ready_SceneMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;
    m_pGraphicDev->AddRef();

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

void CSceneMgr::Render_SceneMgr()
{ }

void CSceneMgr::Change_SceneMgr(SCENETYPE typeInput)
{
    _bool     bFailed = false;

    m_SceneType = typeInput;

    switch (m_SceneType)
    {
    case SCENETYPE::SC_MAINMENU:
        if (FAILED(CManagement::GetInstance()
            ->Request_ChangeScene(CMainMenu::Create(m_pGraphicDev))))
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
    
    case SCENETYPE::SC_TARO_TEST:
        if (FAILED(CManagement::GetInstance()->Request_ChangeScene(CStage_TaroTest::Create(m_pGraphicDev))))
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

    default:
        break;
    }
}

void CSceneMgr::Free()
{
    Safe_Release(m_pGraphicDev);
}
