#include "pch.h"
#include "CTerrain.h"
#include "CCombatStat.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CFloaterMonster.h"
#include "CHumanMonster.h"
#include "CLayer.h"
#include "CMonsterBase.h"
#include "CMonsterSpawner.h"
#include "CRenderer.h"
#include "CTerrainRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"
#include "CWall.h"
#include "CWallRenderer.h"

CTerrain::CTerrain(DEVICE GraphicDev)
    : CRenderObject(GraphicDev), m_BufferCom(nullptr), m_TextureCom284(nullptr),
      m_TextureCom1024(nullptr),
      m_TextureComOutline(nullptr),
      m_TerrainRendererCom(nullptr),
      m_WallRendererCom(nullptr), m_WallSpawn(false)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CRenderObject(rhs), m_BufferCom(nullptr), m_TextureCom284(nullptr),
      m_TextureCom1024(nullptr),
      m_TextureComOutline(nullptr),
      m_TerrainRendererCom(nullptr),
      m_WallRendererCom(nullptr), m_WallSpawn(false)
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 0.f));

    return S_OK;
}

void CTerrain::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

}

_int CTerrain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    // 벽 소환, 배치
    {
        if (m_WallSpawn)
        {
            auto layer = CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::ENVIRONMENT);
            if (layer)
            {
                Spawn_Walls(layer);
                m_WallSpawn = false;
            }
        }

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

        if (m_WallRendererCom && m_WallRendererCom->Get_EditMode())
        {
            _int cellX, cellZ;

            if (m_TerrainRendererCom && m_TerrainRendererCom->PickCellMousePos(cellX, cellZ))
            {
                auto input = CDInputMgr::GetInstance();

                ImGuiIO& io = ImGui::GetIO();
                if (!io.WantCaptureMouse)
                {
                    if (input->MousePress(DIM_LB))
                    {
                        m_WallRendererCom->Set_WallPosition(cellX, cellZ, true);
                    }

                    else if (input->MousePress(DIM_RB))
                    {
                        m_WallRendererCom->Set_WallPosition(cellX, cellZ, false);
                    }
                }
            }
        }
    }
    
    // 몬스터 소환, 배치
    if (m_MonsterSpawn)
    {
        auto layer = CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::GAMELOGIC);
        if (layer)
        {
            Spawn_Monsters(layer);
            m_MonsterSpawn = false;
        }
    }
    // 몬스터 에디터 배치
    if (m_MonsterSpawnerCom && m_MonsterSpawnerCom->Get_EditMode())
    {
        _int cellX, cellZ;

        if (m_TerrainRendererCom && m_TerrainRendererCom->PickCellMousePos(cellX, cellZ))
        {
            auto input = CDInputMgr::GetInstance();

            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse)
            {
                if (input->MousePress(DIM_LB))
                {
                    MONSTER_TYPE selectedType = m_MonsterSpawnerCom->Get_SelectedType();
                    m_MonsterSpawnerCom->Add_SpawnPoint(cellX, cellZ, selectedType);
                }
                else if (input->MousePress(DIM_RB))
                {
                    m_MonsterSpawnerCom->Remove_SpawnPoint(cellX, cellZ);
                }
            }
        }
    }


    return exit;
}

void CTerrain::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

}

void CTerrain::Render_GameObject()
{
    if (m_TerrainRendererCom == nullptr)
        return;

    m_TerrainRendererCom->Render_TerrainRenderer();
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    m_BufferCom = CreateProtoComponent<CTerrainTex>(this, COMPONENTTYPE::TERRAIN_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TERRAIN_TEX, m_BufferCom });

    // Tile TextureComponent
    {
        m_TextureCom284 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_284);
        NULL_CHECK_RETURN(m_TextureCom284, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_284, m_TextureCom284 });

        m_TextureCom1024 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_1024);
        NULL_CHECK_RETURN(m_TextureCom1024, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_1024, m_TextureCom1024 });

        m_TextureComOutline = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_OUTLINE);
        NULL_CHECK_RETURN(m_TextureComOutline, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_OUTLINE, m_TextureComOutline });
    }

    // Terrain Renderer
    m_TerrainRendererCom = CreateProtoComponent<CTerrainRenderer>(this, COMPONENTTYPE::TERRAIN_RENDER);
    NULL_CHECK_RETURN(m_TerrainRendererCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TERRAIN_RENDER, m_TerrainRendererCom });

    FAILED_CHECK_RETURN(m_TerrainRendererCom->Ready_TerrainRenderer(), E_FAIL);

    // Wall Renderer
    m_WallRendererCom = CreateProtoComponent<CWallRenderer>(this, COMPONENTTYPE::WALL_RENDERER);
    NULL_CHECK_RETURN(m_WallRendererCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::WALL_RENDERER , m_WallRendererCom });

    m_WallRendererCom->Initialize(
        m_TerrainRendererCom->Get_SizeX(),
        m_TerrainRendererCom->Get_SizeZ(),
        m_TerrainRendererCom->Get_CellSize());

    // Monster Spawner -> Wall Renderer랑 비슷하게 해보기
    m_MonsterSpawnerCom = CreateProtoComponent<CMonsterSpawner>(this, COMPONENTTYPE::MONSTER_SPAWNER);
    NULL_CHECK_RETURN(m_MonsterSpawnerCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::MONSTER_SPAWNER, m_MonsterSpawnerCom });

    m_MonsterSpawnerCom->Initialize(
        m_TerrainRendererCom->Get_SizeX(),
        m_TerrainRendererCom->Get_SizeZ(),
        m_TerrainRendererCom->Get_CellSize());

    return S_OK;
}

void CTerrain::Render_Editor()
{
    CRenderObject::Render_Editor();

    //ImGui::Text("=== Terrain ===");

}

void CTerrain::LoadMapOnStart(const char* filePath)
{
    if (m_TerrainRendererCom)
    {
        m_TerrainRendererCom->LoadMap(filePath);

        m_WallSpawn = true;         // 벽 생성
        m_MonsterSpawn = true;      // 몬스터 생성
    }
}

void CTerrain::Spawn_Walls(CLayer* layer)
{
    if (!m_WallRendererCom || !m_TerrainRendererCom || !layer)
        return;

    const int sizeX = m_WallRendererCom->Get_GridSizeX();
    const int sizeZ = m_WallRendererCom->Get_GridSizeZ();
    _float cellSize = m_TerrainRendererCom->Get_CellSize();

    int wallCount = 0;

    for (_int z = 0; z < sizeZ - 1; z++)
    {
        for (_int x = 0; x < sizeX - 1; x++)
        {
            if (m_WallRendererCom->Has_Wall(x, z))
            {
                CWall* wall = CWall::Create(m_GraphicDev);

                _vec3 wallPos = m_TerrainRendererCom->GetCellCenterWorld(x, z);

                auto wallTransform = dynamic_cast<CTransform*>(
                    wall->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (wallTransform)
                    wallTransform->Set_Pos(wallPos);

                auto wallCollider = dynamic_cast<CBoxCollider*>(
                    wall->Get_Component(ID_DYNAMIC, COMPONENTTYPE::BOX_COLL));

                if (wallCollider)
                {
                    wallCollider->Set_Size(_vec3(cellSize, 2.f, cellSize));

                    layer->Add_GameObject(OBJTYPE::WALL, wall);

                    wall->Initialize_GameObject();

                    wallCount++;
                }
            }
        }
    }
}

_vec3 CTerrain::Get_CellCenterWorld(_int cellX, _int cellZ)
{
    if (!m_TerrainRendererCom)
        return _vec3(0.f, 0.f, 0.f);

    return m_TerrainRendererCom->GetCellCenterWorld(cellX, cellZ);
}

_float CTerrain::Get_CellSize()
{
    if (!m_TerrainRendererCom)
        return 1.f;

    return m_TerrainRendererCom->Get_CellSize();
}

void CTerrain::Spawn_Monsters(CLayer* layer)
{
    if (!m_MonsterSpawnerCom || !m_TerrainRendererCom || !layer)
        return;

    const auto& spawnPoints = m_MonsterSpawnerCom->Get_SpawnPoints();

    for (const auto& spawn : spawnPoints)
    {
        // 타입에 따라 몬스터 생성
        CMonsterBase* monster = nullptr;
        OBJTYPE objType;

        switch (spawn.monsterType)
        {
        case MONSTER_TYPE::HUMAN_MONSTER:
            monster = CHumanMonster::Create(m_GraphicDev);
            objType = OBJTYPE::MONSTER;
            break;

        case MONSTER_TYPE::FLOATER_MONSTER:
            monster = CFloaterMonster::Create(m_GraphicDev);
            objType = OBJTYPE::MONSTER;
            break;

        default:
            continue;
        }
        if (!monster)
            continue;

        // 위치 설정
        _vec3 spawnPos = m_TerrainRendererCom->GetCellCenterWorld(spawn.cellX, spawn.cellZ);

        auto transform = monster->Get_Transform();
        if (transform)
            transform->Set_Pos(spawnPos);

        // Layer에 추가
        layer->Add_GameObject(objType, monster);

        monster->Initialize_GameObject();

        // HpBar 생성
        auto combat = dynamic_cast<CCombatStat*>(monster->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        if (combat)
        {
            CUI_MonsterHp* monsterHp = CUI_MonsterHp::Create(m_GraphicDev, combat);
            SpawnUIOnLayer(monsterHp, OBJTYPE::UI);

            monsterHp->Initialize_GameObject();
        }
    }
}

CTerrain* CTerrain::Create(DEVICE GraphicDev)
{
    CTerrain* terrain = new CTerrain(GraphicDev);

    if (FAILED(terrain->Ready_GameObject()))
    {
        Safe_Release(terrain);
        MSG_BOX("Create Tile Failed");
        return nullptr;
    }

    return terrain;
}

void CTerrain::Free()
{
    Safe_Release(m_MonsterSpawnerCom); 
    Safe_Release(m_WallRendererCom);

    CRenderObject::Free();
}
