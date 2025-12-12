#pragma once
#include "CMonsterController.h"
#include "CNPCStateMachine.h"

class CNPCController : public CMonsterController
{
private:
    enum class NPC_WORK { NONE, CHOPWOOD, MINING, PRAY };

private:
    explicit CNPCController(DEVICE graphicDev);
    explicit CNPCController(const CNPCController& rhs);
    ~CNPCController() override;

public:
    HRESULT Ready_Controller() override;
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

    // void Render_Editor() override;

public:
    static CNPCController* Create(DEVICE graphicDev);
    CComponent*            Clone() override;

private:
    void Free() override;

public:
    void     Set_Work(NPC_WORK work) { m_Work = work; }
    NPC_WORK Get_Work() const { return m_Work; }

    void         Set_TargetPos(const _vec3& pos) { m_TargetPos = pos; }
    const _vec3& Get_TargetPos() const { return m_TargetPos; }

    void           Set_AnimKeyPrefix(const wstring& prefix) { m_AnimKeyPrefix = prefix; }
    const wstring& Get_AnimKeyPrefix() const { return m_AnimKeyPrefix; }

    // State Helper
    CNPCStateMachine::NPC_STATE Get_CurNPCState() { 
        return static_cast<CNPCStateMachine::NPC_STATE>(Get_CurrentState()); 
    }

    // Interaction Object Setter
    void          Set_TargetObject(CGameObject* obj) { m_TargetObject = obj; }
    CGameObject*  Get_TargetObject()                 { return m_TargetObject; }

private:
    NPC_WORK m_Work = NPC_WORK::NONE;

    _vec3 m_TargetPos = { 0.f, 0.f, 0.f };
    CGameObject* m_TargetObject = nullptr;

    wstring m_AnimKeyPrefix;
};
