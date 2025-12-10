#pragma once

#include "CComponent.h"

enum class MONSTER_TYPE : _int
{
    HUMAN_MONSTER = 0,
    FLOATER_MONSTER,

    MONSTER_TYPE_END
};

class CMonsterSpawner : public CComponent
{
private:
    struct MonsterSpawnData
    {
        _int cellX, cellZ;
        MONSTER_TYPE monsterType;
    };

private:
    explicit CMonsterSpawner(DEVICE graphicDev);
    explicit CMonsterSpawner(const CMonsterSpawner& rhs);
    virtual ~CMonsterSpawner() override;

public:
    HRESULT Ready_MonsterSpawner(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render_Editor() override;

public:
    void Initialize(_int sizeX, _int sizeZ, _float cellSize);

    // 몬스터 배치/제거
    void Add_SpawnPoint(_int x, _int z, MONSTER_TYPE type);
    void Remove_SpawnPoint(_int x, _int z);
    void Clear_All();

    // 조회
    _bool   Has_Monster(_int x, _int z);
    _int    Get_MonsterCount() { return static_cast<_int>(m_SpawnPoints.size()); }
    const vector<MonsterSpawnData>& Get_SpawnPoints() const { return m_SpawnPoints; }

    // 저장/로드
    HRESULT Save_Monsters(ofstream& ofs) const;
    HRESULT Load_Monsters(ifstream& ifs);

    // 에디터
    void Set_EditMode(bool enabled) { m_EditMode = enabled; }
    bool Get_EditMode() const { return m_EditMode; }
    MONSTER_TYPE Get_SelectedType() const { return m_SelectedType; }

public:
    static CMonsterSpawner* Create(DEVICE graphicDev);
    CComponent* Clone() override;

protected:
    void Free() override;

private:
    vector<MonsterSpawnData>    m_SpawnPoints;
    _int                        m_GridSizeX, m_GridSizeZ;
    _float                      m_CellSize;
    _bool                       m_EditMode;
    MONSTER_TYPE                m_SelectedType;

};

