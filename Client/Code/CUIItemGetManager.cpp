#include "pch.h"
#include "CUIItemGetManager.h"

CUIItemGetManager::CUIItemGetManager(DEVICE graphicDev): CGameObject(graphicDev)
{
    
}
CUIItemGetManager::CUIItemGetManager(const CUIItemGetManager& rhs): CGameObject(rhs)
{
    
}
CUIItemGetManager::~CUIItemGetManager()
{
    
}

HRESULT CUIItemGetManager::Ready_GameObject()
{
    return CGameObject::Ready_GameObject();
}

_int CUIItemGetManager::Update_GameObject(const _float& timeDelta)
{
    return CGameObject::Update_GameObject(timeDelta);
}
void CUIItemGetManager::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CUIItemGetManager::Render_GameObject()
{
    for(int i = 1; m_UIItemGets)
    m_UIItemGet

}

void CUIItemGetManager::Render_Editor()
{
    CGameObject::Render_Editor();
}

 void CUIItemGetManager::Free()
{
    CGameObject::Free();
}
