#pragma once
#include "CScene.h"

class CTerrain;
class CUI_Dialogue;     
class CCameraController;

BEGIN(Engine)
class CTransform;
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
    HRESULT Ready_Light() override;

public:
    void    Setup_BossCallbacks();
    void    Setup_DialogueCallbacks();
    void    OnDialogueEnd(const string& id);
    void    OnBoss2DeadEnd();

public:
    static CStage_Boss2* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain*       m_Terrain;
    CGameObject*    m_Boss;

    // Point Light 설정
    _vec3   m_LightPos = { 48.f, 15.f, 40.5f };
    COLOR   m_LightDiffuse = { 1.f, 0.3f, 0.3f, 1.f };
    COLOR   m_LightAmbient = { 0.1f, 0.05f, 0.05f, 1.f };
    COLOR   m_LightSpecular = { 1.f, 0.5f, 0.5f, 1.f };
    _float  m_LightRange = 43.f;
    _float  m_Attenuation0 = 0.f;
    _float  m_Attenuation1 = 0.01f;
    _float  m_Attenuation2 = 0.f;

private:
    CUI_Dialogue*       m_Dialogue = nullptr;
    CCameraController*  m_CamController = nullptr;
    CTransform*         m_PlayerTransform = nullptr;

};
