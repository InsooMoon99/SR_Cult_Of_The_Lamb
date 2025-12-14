#include "pch.h"
#include "CVillageStateMgr.h"

IMPLEMENT_SINGLETON(CVillageStateMgr)

CVillageStateMgr::CVillageStateMgr()
{
}

CVillageStateMgr::~CVillageStateMgr()
{
    CVillageStateMgr::Free();
}

void CVillageStateMgr::Save_Building(const SavedBuilding& buildingInfo)
{
    m_VillageState.savedBuildings.push_back(buildingInfo);
}

void CVillageStateMgr::Update_Building(const _vec3& vPos, _bool bBuilt)
{
    auto& buildings = m_VillageState.savedBuildings;
    for (auto& building : buildings)
    {
        _vec3 diff = building.pos - vPos;
        if (D3DXVec3Length(&diff) < 0.1f) // 위치가 같으면 (오차범위 내)
        {
            building.isBuilt = bBuilt;
            return;
        }
    }
}

void CVillageStateMgr::Save_Interactive(const SavedInteractive& interactiveInfo)
{
    auto& interactives = m_VillageState.savedInteractives;
    
    // 이미 있는지 확인하고 있으면 업데이트
    for (auto& item : interactives)
    {
        _vec3 diff = item.pos - interactiveInfo.pos;
        if (D3DXVec3Length(&diff) < 0.1f)
        {
            item = interactiveInfo;
            return;
        }
    }

    // 없으면 추가
    interactives.push_back(interactiveInfo);
}

SavedInteractive* CVillageStateMgr::Get_InteractiveState(const _vec3& vPos)
{
    auto& interactives = m_VillageState.savedInteractives;
    for (auto& item : interactives)
    {
        _vec3 diff = item.pos - vPos;
        if (D3DXVec3Length(&diff) < 0.1f)
        {
            return &item;
        }
    }

    return nullptr;
}

void CVillageStateMgr::Save_NPC(const wstring& wstrName, _bool bFirstSpawn)
{
    SavedNpc npcInfo;
    npcInfo.name = wstrName;
    npcInfo.firstSpawnAnimPlayed = bFirstSpawn;

    m_VillageState.savedNPCs[wstrName] = npcInfo;
}

SavedNpc* CVillageStateMgr::Get_NPCState(const wstring& wstrName)
{
    auto& npcs = m_VillageState.savedNPCs;
    auto iter = npcs.find(wstrName);

    if (iter != npcs.end())
        return &iter->second;

    return nullptr;
}

void CVillageStateMgr::Free()
{
    // SavedSceneState 내부 멤버(vector/map)는 자동 소멸됨
    m_VillageState.savedBuildings.clear();
    m_VillageState.savedInteractives.clear();
    m_VillageState.savedNPCs.clear();
}
