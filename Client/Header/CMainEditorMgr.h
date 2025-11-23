#pragma once

#include "CBase.h"
#include "CMapObjectBase.h"
#include "CMapEditorPlacer.h"

class CMapObjectBase;
class CMapObjectGrass;

BEGIN(Engine)
class CEditContext;
class CGameObject;
END

class CMainEditorMgr : public CBase
{
    DECLARE_SINGLETON(CMainEditorMgr)

private:
    explicit CMainEditorMgr();
    virtual ~CMainEditorMgr();

public:
    void Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx);

    // 매 프레임 ImGui로 메인 에디터 창 그리는 부분
    void Update_MainEditor(const _float& timeDelta);
    void Render_MainEditor();

    // Map Object Render
    void Render_MapObjectWindow();

private:
    // Context 세팅
    void Set_EditContext(CEditContext* ctx);

    // Editor Render
    void Render_Hierarchy();
    void Render_Inspector();

    void Render_DynamicComponents(CGameObject* obj);
    void Render_StaticComponents(CGameObject* obj);

    void Render_SelectComponent();

    


protected:
    virtual void Free() override;

private:
    DEVICE m_GraphicDev = nullptr;

    CEditContext* m_Context = nullptr;

    // MapObject 배치
    CMapEditorPlacer*   m_MapPlacer = nullptr;

};

