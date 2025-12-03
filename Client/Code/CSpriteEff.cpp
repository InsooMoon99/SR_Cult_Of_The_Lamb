#include "pch.h"
#include "CSpriteEff.h"

CSpriteEff::CSpriteEff()
{
}

CSpriteEff::CSpriteEff(DEVICE graphicDev) : CEffectBaseCom(graphicDev)
, is_SetSpriteDesc(false)
{
}

CSpriteEff::CSpriteEff(const CSpriteEff& rhs) : CEffectBaseCom(rhs)
, is_SetSpriteDesc(rhs.is_SetSpriteDesc)
{
    //컴포넌트는 현구조상 프로토 타입으로 클론하기 떄문에 깊은 복사 , 초기화 필수

}

CSpriteEff::~CSpriteEff()
{
}

HRESULT CSpriteEff::Ready_Effect()
{
    return E_NOTIMPL;
}

void CSpriteEff::Initialize_Component()
{
}

_int CSpriteEff::Update_Component(const _float& timeDelta)
{
    return _int();
}

void CSpriteEff::LateUpdate_Component()
{
}

void CSpriteEff::Render_Effect()
{
}

CSpriteEff* CSpriteEff::Create(DEVICE graphicDev)
{
    auto SpriteEff = new CSpriteEff(graphicDev);

    if (FAILED(SpriteEff->Ready_Effect()))
    {
        MSG_BOX("SpriteEff Create Failed");
        Safe_Release(SpriteEff);
        return nullptr;
    }

    return SpriteEff;
}

CComponent* CSpriteEff::Clone()
{
    return new CSpriteEff(*this);
}

void CSpriteEff::Free()
{
    CEffectBaseCom::Free();
}
