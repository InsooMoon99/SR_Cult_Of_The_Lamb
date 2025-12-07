#pragma once

#include "CBase.h"

class CUI_Fade;

class CSceneMgr : public CBase
{
    DECLARE_SINGLETON(CSceneMgr)

private:
    enum class FADE_STATE
    {
        NONE,
        FADE_OUT,      // 어두워지는 중
        WAITING,       // 완전히 어두워진 상태
        SCENE_CHANGE,  // 씬 전환
        FADE_IN        // 밝아지는 중
    };

private:
    explicit CSceneMgr();
    ~CSceneMgr() override;

public:
    virtual HRESULT Ready_SceneMgr(DEVICE pGraphicDev);
    void            Update_SceneMgr(const _float& timeDelta);
    virtual void    Render_SceneMgr();
    virtual void    Change_SceneMgr(SCENETYPE typeInput);
    void            Change_SceneMgr_WithFade(SCENETYPE typeInput, _float fadeSpeed = 2.0f);


    SCENETYPE       Get_SceneType() { return m_SceneType; }

private:
    void Free() override;
    CUI_Fade* Get_Or_Create_FadeUI();

private:
    DEVICE      m_pGraphicDev;
    SCENETYPE   m_SceneType;

    // 페이드 인, 아웃
    CUI_Fade*   m_FadeUI;
    bool        m_IsFading;
    float       m_FadeTimer;
    SCENETYPE   m_NextSceneType;
    float       m_FadeSpeed;

    FADE_STATE  m_FadeState;
};
