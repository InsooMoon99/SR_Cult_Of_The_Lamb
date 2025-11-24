#include "pch.h"
#include "CDropSystem.h"

#include "CDroppedItem.h"
#include "CGameObject.h"
#include "CInventory.h"
#include "CStage.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CDropSystem)

CDropSystem::CDropSystem()
{ }

CDropSystem::~CDropSystem()
{
    CDropSystem::Free();
}

void CDropSystem::SpawnDrop(DEVICE pGraphicDev, const ItemInstance& itemInst, const _vec3& worldPos)
{
    CDroppedItem* pDrop = CDroppedItem::Create(pGraphicDev, itemInst, worldPos);
    m_nowScene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, pDrop, OBJTYPE::ITEM);
}

void CDropSystem::SpawnAllFromInven(DEVICE pGraphicDev, CInventory& Inven)
{
    CGameObject* obj = Inven.Get_Owner();
    if (obj == nullptr)
        return;

    auto transfom = dynamic_cast<CTransform*>(obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    _vec3 objPos = transfom->Get_Pos();

    for (int i = 0; i < Inven.GetVector().size(); i++)
    {
        const InventorySlot& slot     = Inven.GetVector()[i];
        ItemInstance         itemInst = slot.itemInst;

        if (itemInst.itemId != -1 && Inven.GetVector()[i].count >= 1)
        {
            int dropCount = slot.count;
            for (int j = 0; j < dropCount; j++)
            {
                SpawnDrop(pGraphicDev, itemInst, objPos);
                Inven.RemoveItem(itemInst.itemId, 1);
            }
        }
    }
}

void CDropSystem::Free()
{}
