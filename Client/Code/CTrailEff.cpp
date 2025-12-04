#include "pch.h"
#include "CTrailEff.h"

CTrailEff::CTrailEff()
{
}

CTrailEff::CTrailEff(DEVICE graphicDev) : CEffectBaseCom(graphicDev)
{
}

CTrailEff::CTrailEff(const CTrailEff& rhs) : CEffectBaseCom(rhs)
{
}

CTrailEff::~CTrailEff()
{
}

HRESULT CTrailEff::Ready_Effect()
{
    return E_NOTIMPL;
}

void CTrailEff::Initialize_Component()
{
}

_int CTrailEff::Update_Component(const _float& timeDelta)
{
    return _int();
}

void CTrailEff::LateUpdate_Component()
{
}
void CTrailEff::Render_Effect()
{
}
CTrailEff* CTrailEff::Create(DEVICE graphicDev)
{
    auto TrailEff = new CTrailEff(graphicDev);

    if (FAILED(TrailEff->Ready_Effect()))
    {
        MSG_BOX("TrailEff Create Failed");
        Safe_Release(TrailEff);
        return nullptr;
    }

    return TrailEff;

}

CComponent* CTrailEff::Clone()
{
    return new CTrailEff(*this);
}

void CTrailEff::Free()
{
    CComponent::Free();
}
