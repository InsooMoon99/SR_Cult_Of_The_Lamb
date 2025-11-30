#pragma once

#include "CBase.h"
#include "Engine_Macro.h"

class CResourceMgr : public CBase
{
    DECLARE_SINGLETON(CResourceMgr)

private:
    explicit CResourceMgr();
    virtual  ~CResourceMgr();

public:
    HRESULT Ready_Resource(DEVICE graphicDev);


private:
    HRESULT Ready_Buffer(DEVICE graphicDev);
    HRESULT Ready_Player(DEVICE graphicDev);
    HRESULT Ready_Monster(DEVICE graphicDev);
    HRESULT Ready_BossP1(DEVICE graphicDev);
    HRESULT Ready_BossP2(DEVICE graphicDev);
    HRESULT Ready_UI(DEVICE graphicDev);
    HRESULT Ready_Item(DEVICE graphicDev);
    HRESULT Ready_Common(DEVICE graphicDev);

    HRESULT Ready_Terrain(DEVICE graphicDev);
    HRESULT Ready_Decoration(DEVICE graphicDev);

public:
    void Free() override;
};

