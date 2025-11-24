#pragma once

#include "CBase.h"
#include "Engine_Define.h"

class CDataMgr : public CBase
{
    DECLARE_SINGLETON(CDataMgr)

private:
    CDataMgr();
    ~CDataMgr();

public:
    HRESULT Ready_Data(DEVICE graphicDev);

private:
    HRESULT Ready_Sound_Resource();
    HRESULT Ready_Common_Resource(DEVICE graphicDev);

    HRESULT Ready_Player_Resource(DEVICE graphicDev);
    HRESULT Ready_Monster_Resource(DEVICE graphicDev);
    HRESULT Ready_Boss_Resource(DEVICE graphicDev);
    HRESULT Ready_Item_Resource(DEVICE graphicDev);
    HRESULT Ready_UI_Resource(DEVICE graphicDev);
    HRESULT Ready_Map_Resource(DEVICE graphicDev);

    HRESULT Ready_Prototype(DEVICE graphicDev);

    void Free();

private:
    DEVICE m_GraphicDev = nullptr;
};

