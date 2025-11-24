#include "CGameObject.h"
#include "CTransform.h"
#include "CComponent.h"

CGameObject::CGameObject(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),
      m_Active(true), m_PendingKill(false), m_Name(L"Default_GameObject")
{
    m_GraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_GraphicDev(rhs.m_GraphicDev),
      m_Active(true), m_PendingKill(false), m_Name(rhs.m_Name)
{
    m_GraphicDev->AddRef();
}

CGameObject::~CGameObject()
{ }

CComponent* CGameObject::Get_Component(COMPONENTID componentID, COMPONENTTYPE componentType)
{
    CComponent* component = Find_Component(componentID, componentType);

    if (nullptr == component)
        return nullptr;

    return component;
}

HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& timeDelta)
{
    for (auto& component : m_Components[ID_DYNAMIC])
    {
        component.second->Update_Component(timeDelta);
    }

    return 0;
}

void CGameObject::LateUpdate_GameObject(const _float& timeDelta)
{
    for (auto& component : m_Components[ID_DYNAMIC])
    {
        component.second->LateUpdate_Component();
    }
}

void CGameObject::Render_GameObject()
{ }

CComponent* CGameObject::Find_Component(COMPONENTID componentID, COMPONENTTYPE componentType)
{
    auto& componentMap = m_Components[componentID];

    auto iter = componentMap.find(componentType);

    if (iter == componentMap.end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for_each(m_Components[i].begin(), m_Components[i].end(), CDeleteMap());
        m_Components[i].clear();
    }

    Safe_Release(m_GraphicDev);
}
