#pragma once
#include "CBase.h"

class CMapEditorPlacer;
class CMapObjectBase;
class CMapObjectGrass;
class CTerrainRenderer;

BEGIN(Engine)
class CEditContext;
class CGameObject;
class CCameraCom;
END

class CMainEditorMgr : public CBase
{
    enum class MAPTOOL_VIEW
    {
        NONE,
        TERRAIN,
        MAP_OBJECT,
    };

    DECLARE_SINGLETON(CMainEditorMgr)

private:
    explicit CMainEditorMgr() = default;
    virtual ~CMainEditorMgr();

public:
    void Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx);

    // 매 프레임 ImGui로 메인 에디터 창 그리는 부분
    void Update_MainEditor(const _float& timeDelta);
    void Render_MainEditor();

    void Set_MainCamera(CCameraCom* cam) { m_MainCamera = cam; }

    _bool IsPlaying() const { return m_IsPlaying; }

private:
    // Context 세팅
    void Set_EditContext(CEditContext* ctx);

    void Render_SceneTab();
    void Render_MapToolTab();

    // Editor Render
    void Render_ToolBar();
    void Render_Hierarchy();
    void Render_Inspector();

    void Render_DynamicComponents(CGameObject* obj);
    void Render_StaticComponents(CGameObject* obj);

    void Render_SelectComponent();

    CTerrainRenderer* Find_TerrainRenderer();

public:
    // Map Object 배치
    void Render_MapObjectWindow(); // Map Object Render

    HRESULT SaveMapObject(const char* filePath);
    HRESULT LoadMapObject(const char* filePath);
    void    Clear_PlacedObjects(); // 로드 전 기존 오브젝트 정리

    void Refresh_MapObjectList();

protected:
    virtual void Free() override;

private:
    vector<string> m_MapObjList {};
    _int m_SelectMapObjIndex = 0;

    char m_MapObjFileName[128] = "MapObject_";

private:
    DEVICE              m_GraphicDev = nullptr;
    CEditContext*       m_Context = nullptr;
    CMapEditorPlacer*   m_MapPlacer = nullptr; // MapObject 배치
    CCameraCom*         m_MainCamera = nullptr;

    _bool               m_IsPlaying = true;

    MAPTOOL_VIEW m_MapToolView = MAPTOOL_VIEW::MAP_OBJECT;
};

