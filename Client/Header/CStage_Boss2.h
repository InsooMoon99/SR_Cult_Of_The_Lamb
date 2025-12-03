#pragma once
#include "CScene.h"

class CTerrain;

BEGIN(Engine)

END

class CStage_Boss2 : public CScene
{
private:
    explicit CStage_Boss2(DEVICE graphicDev);
    ~CStage_Boss2() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

public:
    static CStage_Boss2* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain* m_Terrain;

};
