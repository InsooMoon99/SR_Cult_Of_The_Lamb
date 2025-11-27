#pragma once

#include "CBase.h"
#include "CMapObjectBase.h"

class CMapEditorPlacer;

struct MapObjectInfo
{
    const char* name;
    MAPOBJTYPE  type;
    bool*       showFlag;
};

BEGIN(Engine)

class CMapObjectMgr :public CBase
{
    DECLARE_SINGLETON(CMapObjectMgr)

private:
    explicit CMapObjectMgr();
    virtual  ~CMapObjectMgr();

public:
    void Ready_MapObjectMgr(DEVICE graphicDev);
    void Update_MapObjectMgr(const _float& timeDelta);

    // Map Object 배치
    void    Render_MapObjectWindow();

    void    Render_GrassWindow();
    void    Render_RubbleWindow();
    void    Render_DecoWindow();

    HRESULT SaveMapObject(const char* filePath);
    HRESULT LoadMapObject(const char* filePath);

    void    Clear_PlacedObjects(); // 로드 전 기존 오브젝트 정리
    void    Refresh_MapObjectList();

    CMapEditorPlacer* Get_MapPlacer() const { return m_MapPlacer; }

private:
    inline void Free() override;

private:
    DEVICE              m_GraphicDev = nullptr;
    CMapEditorPlacer*   m_MapPlacer = nullptr;

    vector<string>      m_MapObjList{};

    _int                m_SelectMapObjIndex = 0;

    _int                m_SelectGrassIndex = 0;
    _int                m_SelectRubbleIndex = 0;
    _int                m_SelectDecoIndex = 0;

    _bool               m_ShowGrass = false;
    _bool               m_ShowRubble = false;
    _bool               m_ShowDeco = false;

    char                m_MapObjFileName[128] = "MapObject_";


};

END
