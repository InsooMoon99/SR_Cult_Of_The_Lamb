#pragma once

#include "CMonsterBase.h"

class CUI_BossHp;
class CMonsterController;
class CBoss2_StateMachine;
class CEyeball;

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
        PHASE2_BULLET1,
        PHASE2_BULLET2,
        PHASE2_GROGGY,
        PHASE2_DEAD,
        PHASE2_FOLLOWER,

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

    void				Set_DeathCount(_int count) { m_Eyeball_DeathCount = count; }
    _int				Get_DeathCount() { return m_Eyeball_DeathCount; }

    void				Increase_DeathCount(_int value = 1) { m_Eyeball_DeathCount += value; }

    const vector<CEyeball*>& Get_Eyeballs() { return m_Eyeballs; }
public:
	void Add_Eyeball(CEyeball* eyeball);
	void Remove_Eyeball(CEyeball* eyeball);
	void Command_Ground(); // Eyeball 땅으로 들어가도록 명령

public:
	void                Add_Eyeball(CEyeball* eyeball);
	void                Remove_Eyeball(CEyeball* eyeball);
	void                Command_Ground(); // Eyeball 땅으로 들어가도록 명령

    _bool               Is_InSpecialPattern();

public:
    using               OnBattleStartCallback = function<void()>;
    void                Set_OnBattleStart(OnBattleStartCallback callback) { m_OnBattleStart = callback; };

    using               OnDeadCallback = function<void()>;
    void                Set_OnDead(OnDeadCallback callback) { m_OnDead = callback; };

private:
    OnBattleStartCallback   m_OnBattleStart;
    OnDeadCallback          m_OnDead;

public:
    static CBossPhase2*     Create(DEVICE graphicDev);

protected:
    void    Animation_Setting() override;
    HRESULT Add_Component() override;

protected:
    void    Free() override;

public:
    using OnFollowerStartCallback = function<void()>;

    void  Set_OnFollowerStart(OnFollowerStartCallback callback) { m_OnFollowerStart = callback; }
    void  Trigger_OnFollowerStart() { if (m_OnFollowerStart) m_OnFollowerStart(); }
private:
    OnFollowerStartCallback m_OnFollowerStart = nullptr;

private:
    CMonsterController*     m_BossController;
    _int                    m_Eyeball_DeathCount = 0;

	vector<CEyeball*>		m_Eyeballs;

};

