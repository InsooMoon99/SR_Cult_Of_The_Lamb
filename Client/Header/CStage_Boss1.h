#pragma once

#include "CScene.h"

class CTerrain;
class CUI_Dialogue;
class CCameraController;

class CStage_Boss1 : public CScene
{
private:
    explicit CStage_Boss1(DEVICE graphicDev);
    ~CStage_Boss1() override;

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

    void    OnDialogueEnd(const string& id);

    void    Create_Trigger(_vec3 pos);
    void    Begin_BossIntro();  // 카메라 연출

public:
    static CStage_Boss1* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain*           m_Terrain;
    CGameObject*        m_Boss;

    CUI_Dialogue*       m_Dialogue;
    CCameraController*  m_CamController;

};
