#pragma once
#include "CComponent.h"
#include "CItemDB.h"
#include "ItemInstance.h"

BEGIN(Engine)
class CScene;
END

struct InventorySlot
{
    ItemType     SlotType;        // Material, Food, Weapon…
    ItemInstance itemInst;    // 들어 있는 아이템 (없으면 id = -1)
    int          count = 0;
};

class CInventory : public CComponent
{
private:
    explicit CInventory(DEVICE graphicDev);

    explicit CInventory(const CInventory& rhs);
    ~CInventory() override;

public:
    virtual HRESULT Ready_Inventory();
    void            SetInvenSlotNum(int slotNum); //무조건 만들어야함 

    void DropItemFromCount(int DropItemNum, _vec3 DropPos, CScene* nowScene);

    void DropItemfromSlot(int slotNum, _vec3 DropPos);

    // void DropItem(int DropItemNum); // 오너가 죽엇을떄 이거 호출하면 드롭 아이템으로 -1 이면 전부
    bool AddItem(ItemInstance itemInst);
    int  FindEmptySlot(ItemType itemtype);
    bool RemoveItem(int itemID, int count);

    bool RemoveItemBySlot(int slotNum, int count);

    const vector<InventorySlot>& GetVector() const { return m_vecInven; }
public:
    static CInventory* Create(DEVICE graphicDev);
    CComponent*        Clone() override;
private:
    vector<InventorySlot> m_vecInven;
    int                   invenSlotNum;

public:
    void Free() override;
};
