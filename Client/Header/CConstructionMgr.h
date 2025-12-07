#pragma once
#include "CBase.h"

class CConstructionPlacer;

class CConstructionMgr : public CBase
{
    DECLARE_SINGLETON(CConstructionMgr)

private:
    explicit CConstructionMgr();
    ~CConstructionMgr() override;

public:
    void Ready_ConstructionMgr(DEVICE graphicDev);
    void Update_ConstructionMgr(const _float& fTimeDelta);
    void Render_ConstructionMgr();

    // 건설 모드 제어
    // iItemIndex: 건설할 아이템의 인벤토리 인덱스 혹은 ID
    void Start_BuildMode(BUILDING_TYPE type);
    void End_BuildMode();

    _bool IsBuilding() const { return m_bIsBuilding; }

private:
    void Free() override;

private:
    DEVICE               m_GraphicDev = nullptr;
    CConstructionPlacer* m_pPlacer    = nullptr;

    _bool m_bIsBuilding = false;
};
