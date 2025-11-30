#include "pch.h"
#include "CNPCBase.h"

CNPCBase::CNPCBase(DEVICE graphicDev)
    : CCharacterBase(graphicDev)
{
}

CNPCBase::CNPCBase(const CNPCBase& rhs)
    : CCharacterBase(rhs)
{
}

CNPCBase::~CNPCBase()
{
}

HRESULT CNPCBase::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;


    return S_OK;
}

_int CNPCBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);


    return exit;
}

void CNPCBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CNPCBase::Free()
{
    CCharacterBase::Free();
}
