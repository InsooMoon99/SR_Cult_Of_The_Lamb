#pragma once
#include "CScene.h"

class CTerrain;

class CStage_Prologue : public CScene
{
private:
    explicit CStage_Prologue(DEVICE graphicDev);
    ~CStage_Prologue() override;

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

    HRESULT Ready_Prototype();

    HRESULT Ready_Light() override;

public:
    static CStage_Prologue* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain* m_Terrain;
};
