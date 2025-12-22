#pragma once
#include "CScene.h"

class CCameraController;
class CTerrain;
class CUI_Dialogue;
class CPlayer;

BEGIN(Engine)

class CTransform;
class CRigidbody;

END

class CStage_DungeonEntrance : public CScene
{
private:
    explicit CStage_DungeonEntrance(DEVICE graphicDev);
    ~CStage_DungeonEntrance() override;

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

    void    Create_Trigger();

public:
    static CStage_DungeonEntrance* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain* m_Terrain;

    CTransform* m_PlayerTransform = nullptr;
    CRigidbody* m_PlayerRigid = nullptr;
    CUI_Dialogue* m_Dialogue = nullptr;
    CGameObject* m_Player = nullptr;

    CCameraController* m_CamController = nullptr;

private:
    // 조명
    D3DXCOLOR m_LightDiffuse = D3DXCOLOR(0.631f, 0.345f, 0.345f, 1.0f);
    D3DXCOLOR m_LightAmbient = D3DXCOLOR(0.710f, 0.463f, 0.463f, 1.0f);
    D3DXCOLOR m_LightSpecular = D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f);
    D3DXVECTOR3 m_LightDir = D3DXVECTOR3(0.f, -1.f, 1.f);

private:
    _bool m_CameraZoomOut = false;
};
