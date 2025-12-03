#pragma once

#include "CBase.h"
#include "Engine_Macro.h"

BEGIN(Engine)
class CTexture;
END


class CResourceMgr : public CBase
{
    DECLARE_SINGLETON(CResourceMgr)

private:
    explicit CResourceMgr();
    virtual  ~CResourceMgr();

public:
    HRESULT Ready_Resource(DEVICE graphicDev);
    HRESULT Ready_BossResource(DEVICE graphicDev);
    HRESULT Ready_BossResource2(DEVICE graphicDev);

public:
    HRESULT Ready_Buffer(DEVICE graphicDev);
    HRESULT Ready_Player(DEVICE graphicDev);
    HRESULT Ready_Monster(DEVICE graphicDev);
    HRESULT Ready_UI(DEVICE graphicDev);
    HRESULT Ready_Item(DEVICE graphicDev);
    HRESULT Ready_Common(DEVICE graphicDev);

    HRESULT Ready_Terrain(DEVICE graphicDev);
    HRESULT Ready_Decoration(DEVICE graphicDev);

public:
    HRESULT Ready_BossP1(DEVICE graphicDev);

    HRESULT Ready_BossP1_Idle_Awake(DEVICE graphicDev, CTexture* bossTex);
    HRESULT Ready_BossP1_Attack_Dead(DEVICE graphicDev, CTexture* bossTex);
    HRESULT Ready_BossP1_Movement(DEVICE graphicDev, CTexture* bossTex);
    HRESULT Ready_BossP1_Groggy(DEVICE graphicDev, CTexture* bossTex);
    HRESULT Ready_BossP1_Effects(DEVICE graphicDev, CTexture* bossTex);
    HRESULT Ready_BossP1_Extra(DEVICE graphicDev);

public:
    HRESULT Ready_BossP2(DEVICE graphicDev);

public:
    void Free() override;


};

