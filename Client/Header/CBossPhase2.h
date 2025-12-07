#pragma once

#include "CMonsterBase.h"

class CMonsterController;
class CBoss2_StateMachine;

class CBossPhase2 : public CMonsterBase
{
public:
    enum PATTERN
    {
        PHASE2_START = 0,
        PHASE2_IDLE,
        PHASE2_ROAR,
        PHASE2_EYEBALL,
        PHASE2_CHAIN,
        PHASE2_BULLET,
        PHASE2_DEAD,

        PHASE2_END

    };

private:
    explicit CBossPhase2(DEVICE graphicDev);
    explicit CBossPhase2(const CBossPhase2& rhs);
    virtual ~CBossPhase2() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    CMonsterController* Get_Controller() { return m_BossController; }
    const char*         Get_Boss2_StateName(_int stateNum);
    DEVICE              Get_GraphicDev() { return m_GraphicDev; }
        
public:
    static CBossPhase2* Create(DEVICE graphicDev);

protected:
    void    Animation_Setting() override;
    HRESULT Add_Component() override;

protected:
    void    Free() override;

private:
    CMonsterController*    m_BossController;

};

