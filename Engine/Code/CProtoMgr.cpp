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
    return Ready_Prototype(componentType, L"", component);
}

HRESULT CProtoMgr::Ready_Prototype(COMPONENTTYPE componentType, const wstring& tag, CComponent* component)
{
    ProtoKey key{ componentType, tag };

    auto iter = m_Prototypes.find(key);
    if (iter != m_Prototypes.end())
        return E_FAIL;

    m_Prototypes.insert({ key, component });
    return S_OK;
}

CComponent* CProtoMgr::Clone_Prototype(COMPONENTTYPE componentType, const wstring& tag)
{
    CComponent* component = Find_Prototype(componentType, tag);
    if (component == nullptr)
        return nullptr;

    return component->Clone();
}

CComponent* CProtoMgr::Clone_Prototype(COMPONENTTYPE componentType)
{
    return Clone_Prototype(componentType, L"");
}

CComponent* CProtoMgr::Find_Prototype(COMPONENTTYPE componentType)
{
    return Find_Prototype(componentType, L"");
}

CComponent* CProtoMgr::Find_Prototype(COMPONENTTYPE componentType, const wstring& tag)
{
    ProtoKey key{ componentType, tag };
    auto iter = m_Prototypes.find(key);

    if (iter == m_Prototypes.end())
        return nullptr;

    return iter->second;
}

void CProtoMgr::Free()
{
    for_each(m_Prototypes.begin(), m_Prototypes.end(), CDeleteMap());
    m_Prototypes.clear();
}
