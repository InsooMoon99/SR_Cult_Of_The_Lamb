#include "pch.h"
#include "CGameObject.h"
#include "CUIItemGetManager.h"
#include "CUIItemGet.h"
#include "CInventory.h"

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
    return S_OK;
}
_int CUIItemGetManager::Update_GameObject(const _float& timeDelta)
{
    int exit = CGameObject::Update_GameObject(timeDelta);

    m_Inventory->SetOnItemAdded([this](ItemInstance* itemInstance, int itemCount) {this->Add_UIItemGet(itemInstance, itemCount); });

    return exit;
}
void CUIItemGetManager::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CUIItemGetManager::Render_GameObject()
{

}

void CUIItemGetManager::Render_Editor()
{
    CGameObject::Render_Editor();
}


void CUIItemGetManager::Free()
{
    CGameObject::Free();
}

void CUIItemGetManager::Add_UIItemGet(ItemInstance* itemInstance, int itemCount)

{
    m_UIItemGet = CUIItemGet::Create(m_GraphicDev);
    m_UIItemGet->Set_UIItem(itemInstance->itemId, itemCount);

    m_UIItemGets.push_back(m_UIItemGet);

}
CUIItemGetManager* CUIItemGetManager::Create(DEVICE graphicDev)
{

    return nullptr;
}
