#include "pch.h"
#include "CMonsterSpawner.h"
#include "CCreateHelper.h"
#include "CTerrain.h"

CMonsterSpawner::CMonsterSpawner(DEVICE graphicDev)
    : CComponent(graphicDev)
    , m_GridSizeX(0)
    , m_GridSizeZ(0)
    , m_CellSize(1.f)
    , m_EditMode(false)
    , m_SelectedType(MONSTER_TYPE::HUMAN_MONSTER)
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& rhs)
    : CComponent(rhs)
    , m_SpawnPoints(rhs.m_SpawnPoints)
    , m_GridSizeX(rhs.m_GridSizeX)
    , m_GridSizeZ(rhs.m_GridSizeZ)
    , m_CellSize(rhs.m_CellSize)
    , m_EditMode(false)
    , m_SelectedType(MONSTER_TYPE::HUMAN_MONSTER)
{
}

CMonsterSpawner::~CMonsterSpawner()
{
}

HRESULT CMonsterSpawner::Ready_MonsterSpawner(DEVICE graphicDev)
{
    return S_OK;
}

_int CMonsterSpawner::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);



    return exit;
}

void CMonsterSpawner::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CMonsterSpawner::Render_Editor()
{
    CComponent::Render_Editor();

    ImGui::PushID("MonsterSpawner");
    {
        ImGui::SeparatorText("Monster Spawner");
        
        ImGui::Checkbox("Edit Mode", &m_EditMode);

        if (m_EditMode)
        {
            // 몬스터 타입 선택
            const char* items[] = { "Human Monster", "Floater Monster" };
            int currentIdx = static_cast<int>(m_SelectedType);

            if (ImGui::Combo("Monster Type", &currentIdx, items, 2))
            {
                m_SelectedType = static_cast<MONSTER_TYPE>(currentIdx);
            }
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "LMB: Place | RMB: Remove");
        }

        ImGui::Separator();

        _int monsterCount = Get_MonsterCount();

        ImGui::Text("Total Monsters : %d", monsterCount);
        if (ImGui::Button("Remove All"))
        {
            Clear_All();
        }
        
        if (m_Owner)
        {
            CTerrain* terrain = dynamic_cast<CTerrain*>(m_Owner);
            if (terrain)
            {
                for (const auto& spawn : m_SpawnPoints)
                {
                    _vec3 spawnPos = terrain->Get_CellCenterWorld(spawn.cellX, spawn.cellZ);

                    DWORD color = (spawn.monsterType == MONSTER_TYPE::HUMAN_MONSTER)
                        ? D3DCOLOR_ARGB(255, 255, 0, 0)    // 빨강
                        : D3DCOLOR_ARGB(255, 0, 255, 255); // cyan

                    Engine::DrawDebugSphere(m_GraphicDev, spawnPos, 0.5f, color);
                }
            }
        }
    }
    ImGui::PopID();
}

void CMonsterSpawner::Initialize(_int sizeX, _int sizeZ, _float cellSize)
{
    m_GridSizeX = sizeX;
    m_GridSizeZ = sizeZ;
    m_CellSize = cellSize;

    m_SpawnPoints.clear();
}

void CMonsterSpawner::Add_SpawnPoint(_int x, _int z, MONSTER_TYPE type)
{
    const _int cellCntX = m_GridSizeX - 1;
    const _int cellCntZ = m_GridSizeZ - 1;

    if (x < 0 || x >= cellCntX || z < 0 || z >= cellCntZ)
        return;

    // 이미 있는지 확인
    for (auto& spawn : m_SpawnPoints)
    {
        if (spawn.cellX == x && spawn.cellZ == z)
        {
            // 같은 위치면 타입만 교체
            spawn.monsterType = type;
            return;
        }
    }

    // 새로 추가
    MonsterSpawnData data;
    data.cellX = x;
    data.cellZ = z;
    data.monsterType = type;

    m_SpawnPoints.push_back(data);
}

void CMonsterSpawner::Remove_SpawnPoint(_int x, _int z)
{
    for (auto it = m_SpawnPoints.begin(); it != m_SpawnPoints.end(); ++it)
    {
        if (it->cellX == x && it->cellZ == z)
        {
            m_SpawnPoints.erase(it);
            return;
        }
    }
}

void CMonsterSpawner::Clear_All()
{
    m_SpawnPoints.clear();
}

_bool CMonsterSpawner::Has_Monster(_int x, _int z)
{
    for (const auto& spawn : m_SpawnPoints)
    {
        if (spawn.cellX == x && spawn.cellZ == z)
            return true;
    }

    return false;
}

HRESULT CMonsterSpawner::Save_Monsters(ofstream& ofs) const
{
    if (!ofs.is_open())
        return E_FAIL;

    // 개수 저장
    uint32_t count = static_cast<uint32_t>(m_SpawnPoints.size());
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));
    // 각 스폰 포인트 저장
    for (const auto& spawn : m_SpawnPoints)
    {
        ofs.write(reinterpret_cast<const char*>(&spawn.cellX), sizeof(_int));
        ofs.write(reinterpret_cast<const char*>(&spawn.cellZ), sizeof(_int));
        ofs.write(reinterpret_cast<const char*>(&spawn.monsterType), sizeof(MONSTER_TYPE));
    }
    return S_OK;
}

HRESULT CMonsterSpawner::Load_Monsters(ifstream& ifs)
{
    if (!ifs.is_open())
        return E_FAIL;
    m_SpawnPoints.clear();

    uint32_t count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

    for (uint32_t i = 0; i < count; ++i)
    {
        MonsterSpawnData spawn;
        ifs.read(reinterpret_cast<char*>(&spawn.cellX), sizeof(_int));
        ifs.read(reinterpret_cast<char*>(&spawn.cellZ), sizeof(_int));
        ifs.read(reinterpret_cast<char*>(&spawn.monsterType), sizeof(MONSTER_TYPE));

        m_SpawnPoints.push_back(spawn);
    }

    return S_OK;
}

CMonsterSpawner* CMonsterSpawner::Create(DEVICE graphicDev)
{
    CMonsterSpawner* spawner = new CMonsterSpawner(graphicDev);

    if (FAILED(spawner->Ready_MonsterSpawner(graphicDev)))
    {
        Safe_Release(spawner);
        MSG_BOX("Create MonsterSpawner Failed");
        return nullptr;
    }

    return spawner;
}

CComponent* CMonsterSpawner::Clone()
{
    return new CMonsterSpawner(*this);
}

void CMonsterSpawner::Free()
{
    CComponent::Free();
}
