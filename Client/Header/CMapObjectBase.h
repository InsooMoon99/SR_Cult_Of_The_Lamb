#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CTerrain;
END

enum class MAPOBJTYPE
{
    GRASS, ROCK, /* Object 종류 추가하기 */ MAPOBJ_END,
};

typedef struct tagMapData
{
    MAPOBJTYPE type;
    _vec3 pos;
    _vec3 scale;

} MapObjectData;

class CMapObjectBase : public CGameObject
{
protected:
    explicit CMapObjectBase(DEVICE graphicDev);
    explicit CMapObjectBase(const CMapObjectBase& rhs);
    ~CMapObjectBase() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    // 에디터 배치용 함수들
    void Set_WorldPos(const _vec3& pos);
    void Set_AttachToTerrain(bool attach) { m_AttachTerrain = attach; }

    void Set_Cell(_int x, _int z)
    {
        m_CellX = x;
        m_CellZ = z;
    }

    void Set_Terrain(Engine::CTerrain* terrain) { m_Terrain = terrain; }

    virtual MAPOBJTYPE Get_Type() abstract;

protected:
    HRESULT Add_BaseComponent(COMPONENTTYPE InType);

protected:
    void Free() override;

protected:
    CRcTex*     m_BufferCom;
    CTransform* m_TransformCom;
    CTexture*   m_TextureCom;

    _bool m_AttachTerrain;
    _int  m_CellX = -1;
    _int  m_CellZ = -1;

    Engine::CTerrain* m_Terrain;
};
