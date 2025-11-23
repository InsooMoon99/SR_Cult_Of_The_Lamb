#include "pch.h"
#include "CMainEditorMgr.h"
#include "CComponent.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CEditContext.h"
#include "CEnumHelper.h"
#include "CManagement.h"
#include "CScene.h"
#include "CState.h"
#include "CCombatStat.h"
#include "CComInspectors.h"
#include "CTerrainRenderer.h"
#include "CTransform.h"
#include "CBoxCollider.h"
#include "CInventory.h"
#include "CWeaponEquip.h"
#include "CClient_CreateHelper.h"
#include "CMapEditorPlacer.h"
#include "CTimerMgr.h"

IMPLEMENT_SINGLETON(CMainEditorMgr)

CMainEditorMgr::~CMainEditorMgr()
{
    Free();
}

void CMainEditorMgr::Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx)
{
    m_GraphicDev = graphicDev;

    Set_EditContext(ctx);

    m_MapPlacer = CMapEditorPlacer::Create(m_GraphicDev);
}

void CMainEditorMgr::Update_MainEditor(const _float& timeDelta)
{
    if (m_MapPlacer)
    {
        m_MapPlacer->Update_MapPlacer(timeDelta);
    }
}

void CMainEditorMgr::Set_EditContext(CEditContext* ctx)
{
    Safe_Release(m_Context);

    m_Context = ctx;
}

void CMainEditorMgr::Render_MainEditor()
{
    if (!m_Context)
        return;

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (scene)
        m_Context->Set_ActiveScene(scene);

    else
        MSG_BOX("Scene Is Where ? ");

    // 프리뷰 오브젝트 렌더
    if (m_MapPlacer)
    {
        m_MapPlacer->Render_Privew();
    }

    ImGui::Begin("MainEditor");

    ImGui::BeginChild("[ Tool bar ]", ImVec2(0, 40), true);
    Render_ToolBar();
    ImGui::EndChild();

    ImGui::Separator();

    // 왼쪽 Hierarchy (계층 구조)
    ImGui::BeginChild("[ Hierarchy ]", ImVec2(170, 0), true);
    Render_Hierarchy();
    ImGui::EndChild();

    ImGui::SameLine();

    // 오른쪽 Inspector (정보 및 설정)
    ImGui::BeginChild("[ Inspector ]", ImVec2(0, 0), true);
    Render_Inspector();
    ImGui::EndChild();

    ImGui::End();
}

void CMainEditorMgr::Render_ToolBar()
{
    float delta = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS60");
    float fps = (delta > 0.f) ? 1.f / delta : 0.f;

    ImGui::Columns(2, "ToolbarCols", false);

    // ============
    // Play / FPS
    // ============
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Game");
        ImGui::SameLine();

        if (ImGui::Button(m_IsPlaying ? "Pause" : "Play", ImVec2(60.f, 0.f)))
            m_IsPlaying = !m_IsPlaying;

        ImGui::SameLine();
        ImGui::Text("FPS : %.1f", fps);
    }

    ImGui::NextColumn();

    // =======
    // Camera
    // =======
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Camera");
        ImGui::SameLine();

        Menu_CameraInspector(m_MainCamera);
    }

    ImGui::Columns(1);
}


void CMainEditorMgr::Render_Hierarchy()
{
    CScene* scene = m_Context->Get_ActiveScene();
    if (scene == nullptr)
    {
        ImGui::Text("No Active Scene");
        return;
    }

    if (ImGui::TreeNode("Scene"))
    {
        const auto& layers = scene->Get_Layers();

        for (const auto& layerPair : layers)
        {
            LAYERTYPE layerType = layerPair.first;
            CLayer* layer = layerPair.second;

            if (layer == nullptr) continue;

            string layerName = ToString(layerType);

            if (ImGui::TreeNode(layerName.c_str()))
            {
                const auto& objMap = layer->Get_Objects();

                for (const auto& objPair : objMap)
                {
                    OBJTYPE objType = objPair.first;
                    const vector<CGameObject*>& objList = objPair.second;

                    for (CGameObject* obj : objList)
                    {
                        if (!obj) continue;

                        bool isSelected = (obj == m_Context->Get_SelectedObject());

                        const wstring& wName = obj->Get_Name();
                        string name(wName.begin(), wName.end()); // wstring -> string

                        if (ImGui::Selectable(name.c_str(), isSelected))
                        {
                            m_Context->Select_Object(obj);
                        }
                    }
                }

                ImGui::TreePop(); // Layer
            }
        }

        ImGui::TreePop(); // Scene
    }
}

void CMainEditorMgr::Render_Inspector()
{
    CGameObject* obj = m_Context->Get_SelectedObject();

    if (obj == nullptr)
    {
        ImGui::Text("Not Object Selected");
        return;
    }

    // 기본값 Transform으로 시작
    if (m_Context->Get_SelectedComponent() == nullptr)
    {
        CComponent* transformCom = obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM);

        if (transformCom)
            m_Context->Select_Component(transformCom);
    }

    // Object 이름 세팅
    {
        wstring wName = obj->Get_Name();
        string name(wName.begin(), wName.end());
        ImGui::Text("Object : %s", name.c_str());
    }

    ImGui::Separator();
    ImGui::Text("[ Components ]");

    Render_DynamicComponents(obj);
    Render_StaticComponents(obj);

    ImGui::Separator();
    obj->Render_Editor();

    Render_SelectComponent();
}

void CMainEditorMgr::Render_DynamicComponents(CGameObject* obj)
{
    // Dynamic Component
    const auto& dynamicComponents = obj->Get_DynamicComponents();

    for (const auto& typePair : dynamicComponents)
    {
        COMPONENTTYPE   type = typePair.first;
        CComponent* com = typePair.second;

        if (!com) continue;

        wstring wTypeName = com->Get_Name();
        string typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (com == m_Context->Get_SelectedComponent());

        if (ImGui::Selectable(typeName.c_str(), isSelected))
        {
            m_Context->Select_Component(com);
        }
    }
}

void CMainEditorMgr::Render_StaticComponents(CGameObject* obj)
{
    // Static Component
    auto inven = obj->Get_Component(ID_STATIC, COMPONENTTYPE::INVENTORY);
    if (inven)
    {
        wstring wTypeName = inven->Get_Name();
        string typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (inven == m_Context->Get_SelectedComponent());
        if (ImGui::Selectable(typeName.c_str(), isSelected))
        {
            m_Context->Select_Component(inven);
        }
    }

    auto weapon = obj->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP);
    if (weapon)
    {
        wstring wTypeName = weapon->Get_Name();
        string typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (weapon == m_Context->Get_SelectedComponent());
        if (ImGui::Selectable(typeName.c_str(), isSelected))
        {
            m_Context->Select_Component(weapon);
        }
    }
}

void CMainEditorMgr::Render_SelectComponent()
{
    // 선택된 컴포넌트의 인스펙터
    CComponent* selectComponent = m_Context->Get_SelectedComponent();
    if (selectComponent)
    {
        ImGui::Separator();

        string componentName = WStringToUtf8(selectComponent->Get_Name());

        switch (selectComponent->Get_Type())
        {
            // Dynamic
        case COMPONENTTYPE::TRANSFORM:
        {
            auto transform = dynamic_cast<CTransform*>(selectComponent);
            Transform_Inspector(transform);
            break;
        }
        case COMPONENTTYPE::STATE:
        {
            auto state = dynamic_cast<CState*>(selectComponent);
            State_Inspector(state);
            break;
        }
        case COMPONENTTYPE::COMBATSTAT:
        {
            auto stat = dynamic_cast<CCombatStat*>(selectComponent);
            CombatStat_Inspector(stat);
            break;
        }

        case COMPONENTTYPE::CAMERA:
        {
            auto camera = dynamic_cast<CCameraCom*>(selectComponent);
            Camera_Inspector(camera);
            break;
        }

        case COMPONENTTYPE::TERRAIN_RENDER:
        {
            auto terrain = dynamic_cast<CTerrainRenderer*>(selectComponent);
            Terrain_Inspector(terrain);
            break;
        }

        case COMPONENTTYPE::BOX_COLL:
        {
            auto collider = dynamic_cast<CBoxCollider*>(selectComponent);
            BoxColl_Inspector(collider);
            break;
        }

        // Static
        case COMPONENTTYPE::INVENTORY:
        {
            auto inven = dynamic_cast<CInventory*>(selectComponent);
            Inventory_Inspector(inven);
            break;
        }

        case COMPONENTTYPE::WEAPON_EQUIP:
        {
            auto weapon = dynamic_cast<CWeaponEquip*>(selectComponent);
            Weapon_Inspector(weapon);
            break;
        }

        // 다른 컴포넌트 타입 ImGui에 보여줄거 있으면 추가하기

        }
    }
}

void CMainEditorMgr::Render_MapObjectWindow()
{
    ImGui::Begin("Map Objects");

    bool isGrassSelected =
        (m_MapPlacer && m_MapPlacer->IsPlacing() && m_MapPlacer->Get_CurType() == MAPOBJTYPE::GRASS);

    // Grass 풀때기만 일단
    if (ImGui::Selectable("Grass", isGrassSelected))
    {
        if (m_MapPlacer)
            m_MapPlacer->Start_Place(MAPOBJTYPE::GRASS);
    }

    // 추가 예정) Rock, Tree 등등

    // 배치 취소
    if (m_MapPlacer && m_MapPlacer->IsPlacing())
    {
        if (ImGui::Button("Cancel Place"))
        {
            m_MapPlacer->Cancel_Place();
        }
    }

    ImGui::End();
}


void CMainEditorMgr::Free()
{
    Safe_Release(m_MapPlacer);

    Safe_Release(m_Context);
    Safe_Release(m_GraphicDev);
}
