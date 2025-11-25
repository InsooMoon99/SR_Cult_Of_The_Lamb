#include "pch.h"
#include "CMainEditorMgr.h"
#include "CDInputMgr.h"
#include "CBoxCollider.h"
#include "CCameraCom.h"
#include "CClient_CreateHelper.h"
#include "CCombatStat.h"
#include "CComInspectors.h"
#include "CComponent.h"
#include "CEditContext.h"
#include "CEnumHelper.h"
#include "CGameObject.h"
#include "CInventory.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CMapEditorPlacer.h"
#include "CMapObjectBase.h"
#include "CMapObject_Grass.h"
#include "CScene.h"
#include "CState.h"
#include "CTerrainRenderer.h"
#include "CTimerMgr.h"
#include "CTransform.h"
#include "CWeaponEquip.h"

IMPLEMENT_SINGLETON(CMainEditorMgr)

CMainEditorMgr::~CMainEditorMgr()
{
    CMainEditorMgr::Free();
}

void CMainEditorMgr::Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx)
{
    m_GraphicDev = graphicDev;

    Set_EditContext(ctx);

    m_MapPlacer = CMapEditorPlacer::Create(m_GraphicDev);

    Refresh_MapObjectList();
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

void CMainEditorMgr::Render_SceneTab()
{
    ImGui::BeginChild("[ Tool bar ]", ImVec2(0, 40), true);
    Render_ToolBar();
    ImGui::EndChild();

    ImGui::Separator();

    // 왼쪽 Hierarchy
    ImGui::BeginChild("[ Hierarchy ]", ImVec2(170, 0), true);
    Render_Hierarchy();
    ImGui::EndChild();

    ImGui::SameLine();

    // 오른쪽 Inspector
    ImGui::BeginChild("[ Inspector ]", ImVec2(0, 0), true);
    Render_Inspector();
    ImGui::EndChild();
}

void CMainEditorMgr::Render_MapToolTab()
{
    // 공용 툴바
    ImGui::BeginChild("[ MapTool_Toolbar ]", ImVec2(0, 40), true);
    Render_ToolBar();
    ImGui::EndChild();

    ImGui::Separator();

    // =======================
    // 왼쪽 : 메뉴 (선택만 담당)
    // =======================
    ImGui::BeginChild("[ MapTool_Menu ]", ImVec2(140, 0), true);

    ImGui::Text("Map Tools");
    ImGui::Separator();

    bool selTerrain = (m_MapToolView == MAPTOOL_VIEW::TERRAIN);
    bool selMapObject = (m_MapToolView == MAPTOOL_VIEW::MAP_OBJECT);

    if (ImGui::Selectable("Terrain Renderer", selTerrain))
        m_MapToolView = MAPTOOL_VIEW::TERRAIN;

    if (ImGui::Selectable("Map Object", selMapObject))
        m_MapToolView = MAPTOOL_VIEW::MAP_OBJECT;

    ImGui::EndChild();

    // =======================
    // 오른쪽 : 내용 (선택된 메뉴에 따라 변경)
    // =======================
    ImGui::SameLine();

    ImGui::BeginChild("[ MapTool_Content ]", ImVec2(0, 0), true);

    switch (m_MapToolView)
    {
    case MAPTOOL_VIEW::TERRAIN:
    {
        CTerrainRenderer* terrain = Find_TerrainRenderer();
        if (terrain)
        {
            ImGui::Text("Terrain Renderer");
            ImGui::Separator();
            Terrain_Inspector(terrain);
        }
        else
        {
            ImGui::Text("TerrainRenderer 를 찾을 수 없습니다.");
        }
        break;
    }

    case MAPTOOL_VIEW::MAP_OBJECT:
    {
        Render_MapObjectWindow();   // 기존 맵 오브젝트 배치 UI
        break;
    }

    case MAPTOOL_VIEW::NONE:
    default:
        ImGui::Text("왼쪽에서 툴을 선택하세요.");
        break;
    }

    ImGui::EndChild();
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

    if (ImGui::BeginTabBar("MainEditorTabs"))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            Render_SceneTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("MapTool"))
        {
            Render_MapToolTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

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
        if (obj && obj->Is_PendingDestroy() == false)
        {
            wstring wName = obj->Get_Name();
            string name(wName.begin(), wName.end());
            ImGui::Text("Object : %s", name.c_str());
        }
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

CTerrainRenderer* CMainEditorMgr::Find_TerrainRenderer()
{
    if (!m_Context)
        return nullptr;

    CScene* scene = m_Context->Get_ActiveScene();
    if (!scene)
        return nullptr;

    const auto& layers = scene->Get_Layers();

    for (const auto& layerPair : layers)
    {
        CLayer* layer = layerPair.second;
        if (!layer) continue;

        const auto& objMap = layer->Get_Objects();

        for (const auto& objPair : objMap)
        {
            const vector<CGameObject*>& objList = objPair.second;

            for (CGameObject* obj : objList)
            {
                if (!obj) continue;

                CComponent* com =
                    obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TERRAIN_RENDER);
                if (!com)
                    continue;

                auto terrain = dynamic_cast<CTerrainRenderer*>(com);
                if (terrain)
                    return terrain;
            }
        }
    }

    return nullptr;
}


void CMainEditorMgr::Render_MapObjectWindow()
{
    bool isGrassSelected =
        (m_MapPlacer && m_MapPlacer->IsPlacing() && m_MapPlacer->Get_CurType() == MAPOBJTYPE::GRASS);

    // Grass 풀때기만 일단
    if (ImGui::Selectable("Grass", isGrassSelected))
    {
        if (m_MapPlacer)
            m_MapPlacer->Start_Place(MAPOBJTYPE::GRASS);
    }

    // 추가 예정) Rock, Tree 등등

#pragma region Legacy

     // 배치 취소 -> MapEditorPlacer에서 진행가능
    //if (m_MapPlacer && m_MapPlacer->IsPlacing())
    //{
    //    if (CDInputMgr::GetInstance()->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB))
    //    {
    //        cout << "Check" << endl;
    //        m_MapPlacer->Cancel_Place();
    //    }
    //    //if (ImGui::Button("Cancel Place"))
    //    //{
    //    //    m_MapPlacer->Cancel_Place();
    //    //}
    //}

#pragma endregion

    ImGui::Text(u8"맵 오브젝트 파일명 : "); ImGui::SameLine();
    ImGui::InputText("##MapObjectName", m_MapObjFileName, 128);

    if (ImGui::Button(u8"Save MapObjects"))
    {
        string path = "../Bin/Resource/Data/MapObject/";
        path += m_MapObjFileName;
        path += ".mapobj";

        if (SUCCEEDED(SaveMapObject(path.c_str())))
        {
            Refresh_MapObjectList(); // 저장하고, 새로고침하기
        }
    }

    // 저장 파일 리스트
    if (ImGui::CollapsingHeader(u8"저장된 맵오브젝트"))
    {
        for (auto& fileName : m_MapObjList)
        {
            if (ImGui::Selectable(fileName.c_str()))
            {
                string fullPath = "../Bin/Resource/Data/MapObject/";
                fullPath += fileName;

                LoadMapObject(fullPath.c_str());
            }
        }
    }

}

HRESULT CMainEditorMgr::SaveMapObject(const char* filePath)
{
    // 현재 Scene에서 Enviroment 레이어에 있는 오브젝트 들고오기
    CScene* scene = CManagement::GetInstance()->Get_Scene();
    CLayer* layer_Enviroment = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);

    vector<MapObjectData> objects;

    for (auto& layerPair : layer_Enviroment->Get_Objects())
    {
        const auto& gameObjects = layerPair.second;

        for (auto& obj : gameObjects)
        {
            CMapObjectBase* mapObj = dynamic_cast<CMapObjectBase*>(obj);

            if (mapObj)
            {
                MapObjectData data;
                data.type = mapObj->Get_Type();

                CTransform* objTransform = dynamic_cast<CTransform*>(mapObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (objTransform == nullptr)
                    continue;

                data.pos = objTransform->Get_Pos();

                objects.push_back(data);
            }
        }
    }

    // 저장
    ofstream ofs(filePath, ios_base::binary);
    if (!ofs.is_open())
        return E_FAIL;

    uint32_t magic = 0x4D415042; // 'MAPB'
    uint32_t version = 1;
    uint32_t count = objects.size();

    ofs.write((char*)&magic, sizeof(magic));
    ofs.write((char*)&version, sizeof(version));
    ofs.write((char*)&count, sizeof(count));

    if (count > 0)
    {
        ofs.write((char*)objects.data(),
            sizeof(MapObjectData) * count);
    }

    ofs.close();
    return S_OK;

}

HRESULT CMainEditorMgr::LoadMapObject(const char* filePath)
{
    ifstream ifs(filePath, ios_base::binary);
    if (!ifs.is_open())
        return E_FAIL;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t count = 0;

    ifs.read((char*)&magic, sizeof(magic));
    ifs.read((char*)&version, sizeof(version));
    ifs.read((char*)&count, sizeof(count));

    if (magic != 0x4D415042) // 'MAPB'
    {
        ifs.close();
        return E_FAIL;
    }

    vector<MapObjectData> objectsData(count);
    if (count > 0)
    {
        ifs.read((char*)objectsData.data(),
            sizeof(MapObjectData) * count);
    }
    ifs.close();

    // 기존 오브젝트 정리까지 할 필요는 없을듯? Scene 넘어갈 때 진행하면 될거같다.
    // Clear_PlacedObjects(); 

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return E_FAIL;

    CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
    if (!layer) return E_FAIL;

    for (const auto& data : objectsData)
    {
        CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, data.type);
        if (newObj)
        {
            newObj->Set_WorldPos(data.pos);
            // TODO : 데이터에서 scale값 가져와서 세팅 추가 예정

            layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);
        }
    }

    return S_OK;
}

void CMainEditorMgr::Clear_PlacedObjects()
{

}

void CMainEditorMgr::Refresh_MapObjectList()
{
    m_MapObjList.clear(); // 맵 리스트 비우고, 새로고침 느낌나게

    const char* mapObjDir = "../Bin/Resource/Data/MapObject";

    filesystem::path dirPath(mapObjDir);

    // 폴더 없으면 생성하기
    if (!filesystem::exists(dirPath))
    {
        filesystem::create_directories(dirPath);
        return;
    }

    if (!filesystem::is_directory(dirPath))
        return;

    // 폴더에서 .mapobj 파일들 찾기
    for (const auto& entry : filesystem::directory_iterator(dirPath))
    {
        if (!entry.is_regular_file())
            continue;

        const filesystem::path& p = entry.path();
        if (p.extension() == ".mapobj")
        {
            // 파일명만 저장
            m_MapObjList.push_back(p.filename().string());
        }
    }

    // 정렬
    sort(m_MapObjList.begin(), m_MapObjList.end());

    // 인덱스 조정
    if (m_MapObjList.empty())
        m_SelectMapObjIndex = -1;
    else if (m_SelectMapObjIndex >= (_int)m_MapObjList.size())
        m_SelectMapObjIndex = (_int)m_MapObjList.size() - 1;

}

void CMainEditorMgr::Free()
{
    Safe_Release(m_MapPlacer);

    Safe_Release(m_Context);
    Safe_Release(m_GraphicDev);
}
