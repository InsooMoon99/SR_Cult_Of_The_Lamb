#pragma once
#include "CBase.h"

class CMapEditorPlacer;
class CMapObjectBase;
class CMapObjectGrass;
class CTerrainRenderer;
class CWallRenderer;

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
        WALL,

        MAPTOOL_END
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

    _bool IsGuiVisible() const { return m_ShowImGui; }
    void  Toggle_ImGui() { m_ShowImGui = !m_ShowImGui; }

private:
    // Context 세팅
    void Set_EditContext(CEditContext* ctx);

    void Render_SceneTab();
    void Render_MapToolTab();
    void Render_SceneChangeTab();

    // Editor Render
    void Render_ToolBar();
    void Render_Hierarchy();
    void Render_Inspector();

    void Render_DynamicComponents(CGameObject* obj);
    void Render_StaticComponents(CGameObject* obj);

    void Render_SelectComponent();

    CTerrainRenderer* Find_TerrainRenderer();
    CWallRenderer*    Find_WallRenderer();

protected:
    virtual void Free() override;

private:
    DEVICE              m_GraphicDev = nullptr;
    CEditContext*       m_Context = nullptr;
    CCameraCom*         m_MainCamera = nullptr;

    _bool               m_IsPlaying = true;
    _bool               m_ShowImGui = true;

    MAPTOOL_VIEW        m_MapToolView = MAPTOOL_VIEW::TERRAIN;
};

