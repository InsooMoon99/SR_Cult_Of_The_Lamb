#include "pch.h"
#include "CTerrainRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CTerrainTex.h"
#include "CPickingHelper.h"

static const int ATLAS_COLS_284 = 6;  // 가로 6칸
static const int ATLAS_ROWS_284 = 1;  // 세로 1칸

CTerrainRenderer::CTerrainRenderer(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_TransformCom(nullptr),
      m_BufferCom(nullptr),
      m_TextureCom(nullptr),
      m_SelectedTile(0),
      m_PaintX(0),
      m_PaintZ(0),
      m_MapName{"Terrain_"},
      m_SelectedMapIndex(0), m_SizeX(TILE_CNT_X), m_SizeZ(TILE_CNT_Z), m_CellSize(1.f), m_ShowTile(false),
      m_TileTypeIndex(0)
{
}

CTerrainRenderer::CTerrainRenderer(const CTerrainRenderer& rhs)
    : CComponent(rhs),
      m_TransformCom(nullptr),
      m_BufferCom(nullptr),
      m_TextureCom(nullptr),
      m_TileIndices(rhs.m_TileIndices),
      m_SelectedTile(rhs.m_SelectedTile),
      m_PaintX(rhs.m_PaintX),
      m_PaintZ(rhs.m_PaintZ),
      m_SelectedMapIndex(rhs.m_SelectedMapIndex), m_SizeX(TILE_CNT_X), m_SizeZ(TILE_CNT_Z), m_CellSize(rhs.m_CellSize),
      m_ShowTile(false), m_TileTypeIndex(0),
      m_MapList(rhs.m_MapList)
{
    memcpy(m_MapName, rhs.m_MapName, sizeof(m_MapName));
}

CTerrainRenderer::~CTerrainRenderer()
{
}

HRESULT CTerrainRenderer::Ready_TerrainRenderer()
{
    if (m_Owner)
    {
        m_TransformCom = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        m_BufferCom = dynamic_cast<CTerrainTex*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TERRAIN_TEX));

        m_TextureCom = dynamic_cast<CTexture*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TEX_TILE_284));
    }

    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_BufferCom->AddRef();

    const int iCellCntX = m_SizeX - 1;
    const int iCellCntZ = m_SizeZ - 1;

    m_TileIndices.resize(iCellCntX * iCellCntZ, 0);

    // 초기에 맵 리스트 로드 한번 진행
    Refresh_MapFileList();

    return S_OK;
}

_int CTerrainRenderer::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    // ImGui에 마우스 입력 중일 시에는 에디터 입력 무시
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        int cellX, cellZ;
        if (PickCellMousePos(cellX, cellZ))
        {
            const int iCellCntX = m_SizeX - 1;
            const int index = cellZ * iCellCntX + cellX;

            if (CDInputMgr::GetInstance()->MousePress(DIM_LB))
            {
                m_PaintX = cellX;
                m_PaintZ = cellZ;

                if (index >= 0 && index < (int)m_TileIndices.size())
                    m_TileIndices[index] = m_SelectedTile;
            }
        }
    }

    return exit;
}

void CTerrainRenderer::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CTerrainRenderer::Render_TerrainRenderer()
{
    if (m_BufferCom == nullptr || m_TextureCom == nullptr || m_TransformCom == nullptr)
        return;


    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // 알파블렌딩 안켜주면 투명타일 검은색으로 나옴
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_BufferCom->ApplyTileSetting(m_TileIndices, ATLAS_COLS_284, ATLAS_ROWS_284);

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CTerrainRenderer::Render_Editor()
{
    CComponent::Render_Editor();

	Render_TileWindow();

    if (ImGui::Selectable("Tile_284"))
    {
        m_ShowTile = true;
    }
  
    if (ImGui::Selectable("Tile_Outline"))
    {
		
    }

    ImGui::NewLine();
	ImGui::Separator();
    // --- Save / Load ---
    ImGui::Text(u8"저장할 맵 이름 : ");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputText("##MapName", m_MapName, IM_ARRAYSIZE(m_MapName));

    ImGui::SameLine();
    if (ImGui::Button(u8"Save"))
    {
        string fullPath = "../Bin/Resource/Data/Terrain/";
        fullPath += m_MapName;
        fullPath += ".map";

        if (SUCCEEDED(SaveMap(fullPath.c_str())))
            Refresh_MapFileList();
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader(u8"저장된 맵 리스트", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (!m_MapList.empty())
        {
            for (_int i = 0; i < (_int)m_MapList.size(); ++i)
            {
                bool isSelected = (m_SelectedMapIndex == i);

                if (ImGui::Selectable(m_MapList[i].c_str(), isSelected))
                {
                    m_SelectedMapIndex = i;

                    string fullPath = "../Bin/Resource/Data/Terrain/";
                    fullPath += m_MapList[i];

                    LoadMap(fullPath.c_str());
                }
            }
        }
        else
        {
            ImGui::TextDisabled(u8"[맵 파일 없음]");
        }
    }
    
}

void CTerrainRenderer::Render_TileWindow()
{
	if (!m_ShowTile)
		return;

	ImGui::PushID("Tile_Render");
	{
		if (ImGui::Begin("Tile", &m_ShowTile))
		{
			const char* tileItems[] = { "Tile_284", "Tile_1024" };

			ImGui::Combo("##Combo Tile", &m_TileTypeIndex, tileItems, IM_ARRAYSIZE(tileItems));

			ImGui::Separator();

			if (m_TileTypeIndex == 0)
			{
				Render_Tile284();
			}
			else if (m_TileTypeIndex == 1)
			{
				Render_Tile1024();
			}
		}

		ImGui::End();
	}
	ImGui::PopID();

}

void CTerrainRenderer::Render_Tile284()
{
	ImGui::Text(u8"선택된 타일 : %d", m_SelectedTile + 1);
	ImGui::Separator();

	// --- 타일 버튼 ---
	if (m_TextureCom)
	{
		const _uint  tileCount = 6;
		const _uint  tilePerRow = 6;
		const ImVec2 tileSize(32.f, 32.f);

		IDirect3DBaseTexture9* baseTexture = m_TextureCom->Get_BaseTexture(0);
		if (baseTexture)
		{
			ImTextureID texID = (ImTextureID)baseTexture;

			for (_uint i = 0; i < tileCount; i++)
			{
				if (i > 0 && (i % tilePerRow) != 0)
					ImGui::SameLine();

				_int tileX = i;
				_int tileY = 0;

				_float du = 1.f / (float)ATLAS_COLS_284;
				_float dv = 1.f / (float)ATLAS_ROWS_284;

				ImVec2 uv0(tileX * du, tileY * dv);
				ImVec2 uv1((tileX + 1) * du, (tileY + 1) * dv);

				ImGui::PushID((int)i);

				const _bool isSelected = (i == (_uint)m_SelectedTile);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
				}

				if (ImGui::ImageButton("Tile", texID, tileSize, uv0, uv1))
				{
					m_SelectedTile = (int)i;
				}

				if (isSelected)
					ImGui::PopStyleColor(2);

				ImGui::PopID();
			}
		}

		ImGui::Separator();
		if (ImGui::Button(u8"전체 타일을 선택된 타일로"))
		{
			for (size_t i = 0; i < m_TileIndices.size(); ++i)
				m_TileIndices[i] = m_SelectedTile;
		}
	}
}

void CTerrainRenderer::Render_Tile1024()
{
}

HRESULT CTerrainRenderer::SaveMap(const char* filePath)
{
    ofstream ofs(filePath, ios_base::binary);
    if (!ofs.is_open())
        return E_FAIL;

    uint32_t magic = 0x54494C45; // 'TILE'
    uint32_t version = 2;

    uint32_t cellCntX = m_SizeX - 1;
    uint32_t cellCntZ = m_SizeZ - 1;
    uint32_t tileSize = static_cast<uint32_t>(m_TileIndices.size());
    _float cellSize = m_CellSize;

    ofs.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    ofs.write(reinterpret_cast<char*>(&version), sizeof(version));
    ofs.write(reinterpret_cast<char*>(&cellCntX), sizeof(cellCntX));
    ofs.write(reinterpret_cast<char*>(&cellCntZ), sizeof(cellCntZ));
    ofs.write(reinterpret_cast<char*>(&cellSize), sizeof(cellSize));

    if (tileSize > 0)
    {
        ofs.write(reinterpret_cast<const char*>(m_TileIndices.data()),
            sizeof(_int) * tileSize);
    }

    ofs.close();

    return S_OK;
}

HRESULT CTerrainRenderer::LoadMap(const char* filePath)
{
    ifstream ifs(filePath, ios_base::binary);
    if (!ifs.is_open())
        return E_FAIL;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t cellCntX = 0;
    uint32_t cellCntZ = 0;

    ifs.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    ifs.read(reinterpret_cast<char*>(&version), sizeof(version));
    ifs.read(reinterpret_cast<char*>(&cellCntX), sizeof(cellCntX));
    ifs.read(reinterpret_cast<char*>(&cellCntZ), sizeof(cellCntZ));

    // 파일 포맷 체크
    if (magic != 0x54494C45)
    {
        ifs.close();
        return E_FAIL;
    }

    float cellSize = 1.f;  // 기본값

    if (version >= 2)
    {
        ifs.read(reinterpret_cast<char*>(&cellSize), sizeof(cellSize));
    }

    if (cellCntX + 1 != m_SizeX || cellCntZ + 1 != m_SizeZ || cellSize != m_CellSize)
    {
        Resize_Terrain(cellCntX + 1, cellCntZ + 1, cellSize);
    }

    const size_t tileSize = static_cast<size_t>(cellCntX) * cellCntZ;
    m_TileIndices.resize(tileSize);

    if (tileSize > 0)
    {
        ifs.read(reinterpret_cast<char*>(m_TileIndices.data()),
            sizeof(_int) * tileSize);
    }

    ifs.close();
    return S_OK;
}

_vec3 CTerrainRenderer::GetCellCenterWorld(_uint cellX, _uint cellZ) const
{
    const _float vtxItv = m_CellSize;

    return _vec3(
        cellX * vtxItv + vtxItv * 0.5f,
        0.f,
        cellZ * vtxItv + vtxItv * 0.5f);
}

void CTerrainRenderer::SelectPlaceObject(_uint cellX, _uint cellZ)
{
    const _vec3 pos = GetCellCenterWorld(cellX, cellZ);

    // TODO : 배치할 건물 하나 만들어보기
    //CGameObject* object = 건물 배치?
}

bool CTerrainRenderer::PickCellMousePos(_int& outX, _int& outZ)
{
    _vec3 terrainPos;
    m_TransformCom->Get_Info(INFO_POS, &terrainPos);
    float planeY = terrainPos.y;

	_vec3 hitWorld;
	if (PickPlaneY(m_GraphicDev, g_hWnd, terrainPos.y, hitWorld) == false)
		return false;

	_matrix matWorld = m_TransformCom->Get_World();
	_matrix matInvWorld;
	D3DXMatrixInverse(&matInvWorld, nullptr, &matWorld);

	_vec3 hitLocal;
	D3DXVec3TransformCoord(&hitLocal, &hitWorld, &matInvWorld);

    const float cellSize = m_CellSize;  
    _int cellX = (_int)floorf(hitLocal.x / cellSize);
    _int cellZ = (_int)floorf(hitLocal.z / cellSize);

    if (cellX < 0 || cellX >= m_SizeX - 1 || 
        cellZ < 0 || cellZ >= m_SizeZ - 1)   
        return false;

	outX = cellX;
	outZ = cellZ;
	return true;
}

HRESULT CTerrainRenderer::Refresh_MapFileList()
{
    m_MapList.clear();

    const char* mapDir = "../Bin/Resource/Data/Terrain";

    filesystem::path dirPath(mapDir);
    if (!filesystem::exists(dirPath) || !filesystem::is_directory(dirPath))
        return E_FAIL;

    for (const auto& entry : filesystem::directory_iterator(dirPath))
    {
        if (!entry.is_regular_file())
            continue;

        const filesystem::path& p = entry.path();
        if (p.extension() == ".map")
        {
            // 파일명만 유지 (경로는 제외)
            m_MapList.push_back(p.filename().string());
        }
    }

    // 정렬 한번 후 출력
    sort(m_MapList.begin(), m_MapList.end());

    if (m_MapList.empty())
        m_SelectedMapIndex = -1;

    else if (m_SelectedMapIndex >= (_int)m_MapList.size())
        m_SelectedMapIndex = (_int)m_MapList.size() - 1;

    return S_OK;
}

void CTerrainRenderer::Resize_Terrain(_int sizeX, _int sizeZ, _float cellSize)
{
    m_SizeX = sizeX;
    m_SizeZ = sizeZ;

    m_CellSize = cellSize;

    // 기존 버퍼 메모리 해제
    Safe_Release(m_BufferCom);

    _ulong vtxItv = static_cast<_ulong>(cellSize + 0.5f);
    m_CellSize = static_cast<_float>(vtxItv);

    m_BufferCom = CTerrainTex::Create(m_GraphicDev, sizeX, sizeZ, vtxItv);
}

CTerrainRenderer* CTerrainRenderer::Create(DEVICE graphicDev)
{
    CTerrainRenderer* renderer = new CTerrainRenderer(graphicDev);

    if (renderer == nullptr)
        return nullptr;

    /*if (FAILED(renderer->Ready_TerrainRenderer()))
    {
        Safe_Release(renderer);
        MSG_BOX("Terrain_Renderer Create Faield");
        return nullptr;
    }*/

    return renderer;
}

CComponent* CTerrainRenderer::Clone()
{
    return new CTerrainRenderer(*this);
}

void CTerrainRenderer::Free()
{
    Safe_Release(m_BufferCom);

    CComponent::Free();
}
