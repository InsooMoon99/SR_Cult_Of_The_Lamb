#pragma once
#include "CBase.h"
#include "Client_Struct.h"

// 마을에 있는 오브젝트들의 상태를 저장하는 용도의 매니저 클래스
class CVillageStateMgr : public CBase
{
    DECLARE_SINGLETON(CVillageStateMgr)

private:
    explicit CVillageStateMgr();
    ~CVillageStateMgr() override;

public:
    // Village State Access
    SavedSceneState* Get_State() { return &m_VillageState; }

    // --- Building ---
    void Save_Building(const SavedBuilding& buildingInfo);
    void Update_Building(const _vec3& vPos, _bool bBuilt);
    
    // --- Interactive Object ---
    void Save_Interactive(const SavedInteractive& interactiveInfo);
    SavedInteractive* Get_InteractiveState(const _vec3& vPos);

    // --- NPC ---
    void Save_NPC(const wstring& wstrName, _bool bFirstSpawn);
    SavedNpc* Get_NPCState(const wstring& wstrName);

private:
    void Free() override;

private:
    SavedSceneState m_VillageState;
};
