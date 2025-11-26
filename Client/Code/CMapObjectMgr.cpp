#include "pch.h"
#include "CMapObjectMgr.h"
#include "CClient_CreateHelper.h"
#include "CProtoMgr.h"
#include "CTexture.h"
#include "CMapEditorPlacer.h"
#include "CTransform.h"
#include "CLayer.h"
#include "CScene.h"
#include "CManagement.h"

IMPLEMENT_SINGLETON(CMapObjectMgr)

CMapObjectMgr::CMapObjectMgr()
{

}

CMapObjectMgr::~CMapObjectMgr()
{
    Free();
}

void CMapObjectMgr::Ready_MapObjectMgr(DEVICE graphicDev)
{
    m_GraphicDev = graphicDev;

    m_MapPlacer = CMapEditorPlacer::Create(graphicDev);

    Refresh_MapObjectList();
}

void CMapObjectMgr::Update_MapObjectMgr(const _float& timeDelta)
{
    if (m_MapPlacer)
    {
        m_MapPlacer->Update_MapPlacer(timeDelta);
    }

}

void CMapObjectMgr::Render_MapObjectWindow()
{
    bool isGrassSelected =
        (m_MapPlacer && m_MapPlacer->IsPlacing() && m_MapPlacer->Get_CurType() == MAPOBJTYPE::GRASS);

    // Grass 풀때기만 일단
    if (ImGui::Selectable("Grass", isGrassSelected))
    {
        m_ShowGrass = true;

        //if (m_MapPlacer)
        //    m_MapPlacer->Start_Place(MAPOBJTYPE::GRASS);

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

void CMapObjectMgr::Render_GrassWindow()
{
    if (m_ShowGrass == false)
        return;

    if (ImGui::Begin(u8"풀때기 배치", &m_ShowGrass))
    {
        CTexture* grassTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_GRASS));

        if (grassTexture)
        {
            const _int TOTAL_GRASS = 20;
            const _int COLS = 5;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_GRASS; i++)
            {
                IDirect3DBaseTexture9* baseTexture = grassTexture->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    //_bool isSelected = (i == m_SelectMapObjIndex);
                    //if (isSelected)
                    //{
                    //    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    //}

                    if (ImGui::ImageButton("Grass", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_MapPlacer->Start_Place(MAPOBJTYPE::GRASS);
                        }

                    }

                    //if (isSelected)
                    //{
                    //    ImGui::PopStyleColor();
                    //}

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }

            }
        }

    }

    ImGui::End();
}

HRESULT CMapObjectMgr::SaveMapObject(const char* filePath)
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

HRESULT CMapObjectMgr::LoadMapObject(const char* filePath)
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

void CMapObjectMgr::Clear_PlacedObjects()
{
    // TODO : 클리어 만들어줄 지?
}

void CMapObjectMgr::Refresh_MapObjectList()
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
    {
        m_SelectMapObjIndex = (_int)m_MapObjList.size() - 1;
    }
}

void CMapObjectMgr::Free()
{
    Safe_Release(m_MapPlacer);
    Safe_Release(m_GraphicDev);
}
