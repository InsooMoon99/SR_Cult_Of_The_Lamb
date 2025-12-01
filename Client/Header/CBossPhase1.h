#pragma once

#include "CMonsterBase.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CProtoMgr;
class CRectCollider;
class CBoxCollider;
class CState;
class CCombatStat;
END

class CBossStateMachine;

class CBossPhase1 : public CMonsterBase
{
public:
enum  BOSSPHASE1 { BOSSAWAKE=0, BULLETATTACK, CHAINATTACK, BOSSDEAD, BOSSIDLE, BOSSMOVE, BOSSGROGGRY, BOSS_END };
private:
    explicit CBossPhase1(DEVICE graphicDev);
    explicit CBossPhase1(const CBossPhase1& rhs);
    ~CBossPhase1() override;
public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override {}
    void    OnEndOverlap(CCollider* self, CCollider* other) override {}

    DEVICE  Get_GraphicDev() { return m_GraphicDev; } // 이게  괜찬나..?? 안괜찮을듯

private:
    HRESULT Add_Component() override;
    void    Animation_Setting() override;

    const char* GetBossStateName(_int StateNum);

private:
    CBossStateMachine* m_StateMachine;

public:
    static CBossPhase1* Create(DEVICE graphicDev);

    _vec3 Get_Center() { return BaseCenter; }
    float Get_Length() { return BaseLength; }
private:
    _vec3 StartPos = { 20 ,0, 50 };
    _vec3 BaseCenter;
    int BaseLength = 50;
    float interval = 1.f;
private:
    void Free() override;
};
