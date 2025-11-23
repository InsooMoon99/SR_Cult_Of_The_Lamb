#include "pch.h"
#include "CWeaponEquip.h"
#include "CCombatStat.h"
#include "CItemDB.h"
#include "CPlayer.h"

CWeaponEquip::CWeaponEquip(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_iCurrentWeaponID(-1),
      m_iCurrentWeaponSlot{}
{
    // 빈 슬롯 시작이 -1이면, -1로 맞추는게 안전함.
    m_iCurrentWeaponSlot.itemInst.itemId = -1;
    m_iCurrentWeaponSlot.count = 0;
}

CWeaponEquip::CWeaponEquip(const CWeaponEquip& rhs)
    : CComponent(rhs),
      m_iCurrentWeaponID(-1),
      m_iCurrentWeaponSlot{}
{
    m_iCurrentWeaponSlot.itemInst.itemId = -1;
    m_iCurrentWeaponSlot.count = 0;
}

CWeaponEquip::~CWeaponEquip()
{ }

void CWeaponEquip::Equip_Weapon(ItemInstance itemInst)
{
    if (m_iCurrentWeaponSlot.itemInst.itemId != -1)
        Unequip_Weapon();

    m_iCurrentWeaponSlot.itemInst = itemInst;
    m_iCurrentWeaponID            = itemInst.itemId;

    Item* itemData = CItemDB::GetInstance()->GetItemById(m_iCurrentWeaponID);

    if (itemData)
        ApplyWeaponStat(itemData);
    cout << "Weapon Equipped" << endl;
}

void CWeaponEquip::Unequip_Weapon()
{
    if (m_iCurrentWeaponID == -1)
        return;

    RemoveWeaponStat();

    m_iCurrentWeaponID = -1;
}

void CWeaponEquip::ApplyWeaponStat(Item* item)
{
    auto* player = dynamic_cast<CPlayer*>(Get_Owner());
    if (!player)
        return;

    auto* combatStat = player->Get_CombatStat();

    if (!combatStat)
        return;

    combatStat->Add_Attack(item->stats["attack1"]);

    cout << combatStat->Get_Attack() << endl;

    //dynamic_cast<CPlayer*>(Get_Owner())->Get_CombatStat()->Add_Attack(item->stats["attack1"]);
    //cout << dynamic_cast<CPlayer*>(Get_Owner())->Get_CombatStat()->Get_Attack() << endl;
}

void CWeaponEquip::RemoveWeaponStat()
{
    auto* player = dynamic_cast<CPlayer*>(Get_Owner());
    if (!player)
        return;

    auto* combatStat = player->Get_CombatStat();

    if (!combatStat)
        return;

    Item* itemData = CItemDB::GetInstance()->GetItemById(m_iCurrentWeaponID);

    combatStat->Add_Attack(-(itemData->stats["attack1"]));
}

CWeaponEquip* CWeaponEquip::Create(DEVICE graphicDev)
{
    auto weaponEquip = new CWeaponEquip(graphicDev);

    if (weaponEquip == nullptr)
    {
        MSG_BOX("weaponEquip Create Failed");
        Safe_Release(weaponEquip);
        return nullptr;
    }
    return weaponEquip;
}

void CWeaponEquip::Free()
{
    CComponent::Free();
}

CComponent* CWeaponEquip::Clone()
{
    return new CWeaponEquip(*this);
}
