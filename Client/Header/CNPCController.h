#pragma once
#include "CMonsterController.h"

class CNPCController : public CMonsterController
{
private:
    enum class JOB { UNEMPLOYED, LUMBERJACK, MINER, WORSHIPPER };

private:
    explicit CNPCController(DEVICE graphicDev);
    explicit CNPCController(const CNPCController& rhs);
    ~CNPCController() override;

public:
    HRESULT Ready_Controller() override;
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

    // ImGui or Debug Render if needed
    // void    Render_Editor() override;

public:
    static CNPCController* Create(DEVICE graphicDev);
    CComponent*            Clone() override;

private:
    void Free() override;

public:
    void Set_Job(JOB eJob) { m_eJob = eJob; }
    JOB  Get_Job() const { return m_eJob; }

private:
    JOB m_eJob = JOB::UNEMPLOYED;
};
