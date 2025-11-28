#pragma once

#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTerrainTex;
class CTransform;
class CTexture;
class CLayer;
END

class CTerrainRenderer;
class CWallRenderer;

enum class EDIT_MODE
{
    TILE, OBJECT, EDIT_MODE_END
};

class CTerrain : public CGameObject
{
private:
    explicit CTerrain(DEVICE GraphicDev);
    explicit CTerrain(const CTerrain& rhs);
    virtual ~CTerrain();

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

    void    LoadMapOnStart(const char* filePath);
    void Spawn_Walls(CLayer* layer);

public:
    // 벽만들기
    CWallRenderer* Get_WallRenderer() const { return m_WallRendererCom; }
    _vec3 Get_CellCenterWorld(_int cellX, _int cellZ);
    _float Get_CellSize();

private:
    HRESULT Add_Component();

public:
    static CTerrain* Create(DEVICE GraphicDev);

protected:
    void Free() override;

private:    
    Engine::CTerrainTex*        m_BufferCom;
    Engine::CTransform*         m_TransformCom;

    Engine::CTexture*           m_TextureCom284;
    Engine::CTexture*           m_TextureCom1024;
    Engine::CTexture*           m_TextureComOutline;

    CTerrainRenderer*           m_TerrainRendererCom;
    CWallRenderer*              m_WallRendererCom;

};

