#include "CProtoMgr.h"

#include "CComponent.h"

IMPLEMENT_SINGLETON(CProtoMgr)

CProtoMgr::CProtoMgr()
{ }

CProtoMgr::~CProtoMgr()
{
    CProtoMgr::Free();
}

HRESULT CProtoMgr::Ready_Prototype(COMPONENTTYPE componentType, CComponent* component)
{
    CComponent* instance = Find_Prototype(componentType);

    if (nullptr != instance)
        return E_FAIL;

    m_Prototypes.insert({ componentType, component });

    return S_OK;
}

CComponent* CProtoMgr::Clone_Prototype(COMPONENTTYPE componentType)
{
    CComponent* component = Find_Prototype(componentType);

    if (nullptr == component)
        return nullptr;

    return component->Clone();
}

CComponent* CProtoMgr::Find_Prototype(COMPONENTTYPE componentType)
{
    auto iter = m_Prototypes.find(componentType);

    if (iter == m_Prototypes.end())
        return nullptr;

    return iter->second;
}

void CProtoMgr::Free()
{
    for_each(m_Prototypes.begin(), m_Prototypes.end(), CDeleteMap());
    m_Prototypes.clear();
}
