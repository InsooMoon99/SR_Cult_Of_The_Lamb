#include "pch.h"
#include "CUICharging.h"

CUICharging::CUICharging(DEVICE graphicDev): CUI(graphicDev) {}
CUICharging::CUICharging(const CUICharging& rhs): CUI(rhs) {}
CUICharging::~CUICharging() {}

void CUICharging::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
}

HRESULT CUICharging::Ready_GameObject()
{
    return S_OK;
}

void CUICharging::Render_GameObject()
{
    CUI::Render_GameObject();
}

int CUICharging::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);



    return exit;
}

CUICharging* CUICharging::Create(DEVICE graphicDev)
{
    auto chargingUI = new CUICharging(graphicDev);

    if(FAILED(chargingUI->Ready_GameObject()))
    {
        Safe_Release(chargingUI);
        MSG_BOX("ChargingUI Create Failed");
        return nullptr;
    }
}

void CUICharging::Free()
{
    CUI::Free();
}
