#pragma once
#include "CBase.h"

class CSceneMgr : public CBase
{
    DECLARE_SINGLETON(CSceneMgr)

private:
    explicit CSceneMgr();
    ~CSceneMgr() override;

public:
    virtual HRESULT Ready_SceneMgr(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual void    Render_SceneMgr();
    virtual void    Change_SceneMgr(SCENETYPE typeInput);

    SCENETYPE Get_SceneType() { return m_SceneType; }

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;

    SCENETYPE m_SceneType;

private:
    void Free() override;
};
