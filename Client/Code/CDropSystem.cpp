#include "pch.h"
#include "CDropSystem.h"

#include "CDroppedItem.h"
#include "CGameObject.h"
#include "CInventory.h"
#include "CStage.h"
#include "CTransform.h"
#include "Client_Struct.h"

IMPLEMENT_SINGLETON(CDropSystem)

CDropSystem::CDropSystem()
{ }

CDropSystem::~CDropSystem()
{
    CDropSystem::Free();
}



// 이건 그냥 바닥에 드롭 시킬떄 새로 생성해서 줌
void CDropSystem::SpawnDrop(DEVICE pGraphicDev, const ItemInstance& itemInst, const _vec3& worldPos, std::optional<SpriteBounceDesc> bounceDesc)
{
    CDroppedItem* pDrop = CDroppedItem::Create(pGraphicDev, itemInst, worldPos);
    if (bounceDesc.has_value())
    {
        pDrop->SetBounceDesc(*bounceDesc);//(*bounceDesc) 해야함
    }
    tmpItem.push_back(pDrop);
    m_nowScene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, pDrop, OBJTYPE::ITEM);
}

void CDropSystem::SpawnAllFromInven(DEVICE pGraphicDev, CInventory* Inven)
{
    CGameObject* obj = Inven->Get_Owner();
    if (obj == nullptr)
        return;

    auto transfom = dynamic_cast<CTransform*>(obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    _vec3 ownerPos = transfom->Get_Pos();

    for (int i = 0; i < Inven->GetVector().size(); i++)
    {
        const InventorySlot& slot     = Inven->GetVector()[i];
        ItemInstance         itemInst = slot.itemInst;

        if (itemInst.itemId != -1 && Inven->GetVector()[i].count >= 1)
        {
            int dropCount = slot.count;
            for (int j = 0; j < dropCount; j++)
            {
                int angle = 25*i+ j*13;
                SpriteBounceDesc d;

                _vec3 tmpvec = { cos(D3DXToRadian(angle)),1,sin(D3DXToRadian(angle))};
                D3DXVec3Normalize(&tmpvec, &tmpvec);

                d.pos = ownerPos;
                // d.velocity
                d.velocity = tmpvec * 30;
                d.groundY = 1.f;
                // 중력
                d.gravity = -400.f;
                d.restitution = 0.7f;// 튕김 계수
                d.damping = 0.7f;// 충돌 감쇠(마찰)
                d.m_DampingPerSecond = 0.9f;// 공기 저항
                    _vec3 tmppos = { ownerPos.x, ownerPos.y + 1.5f, ownerPos.z };

                SpawnDrop(pGraphicDev, itemInst, tmppos, d);

                Inven->RemoveItem(itemInst.itemId, 1);
            }
        }
    }
    return ;
}

void CDropSystem::Clear_Item()
{
    for (auto& it : tmpItem)
    {
        if (it != nullptr && (*it).Is_PendingDestroy() != true)
        {
            it->Mark_As_PendingKill();
            
        }
    }
    tmpItem.clear();
}

void CDropSystem::Free()
{
    Clear_Item();
}
