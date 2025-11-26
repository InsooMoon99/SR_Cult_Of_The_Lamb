#pragma once
#include "CGameObject.h"

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

class CBossPhase1 :
    public CGameObject
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

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override {}
    void OnEndOverlap(CCollider* self, CCollider* other) override {}

private:
    HRESULT Add_Component();
    void    Animation_Setting();

private:
    Engine::CRcTex* m_BufferCom;
    Engine::CTransform* m_TransformCom;
    Engine::CTexture* m_TextureCom;
    Engine::CAnimator* m_AnimatorCom;
    Engine::CBoxCollider* m_BoxColCom;
    Engine::CState* m_StateCom;
    Engine::CCombatStat* m_CombatStatCom;

    CBossStateMachine* m_StateMachine;
public:
    static CBossPhase1* Create(DEVICE graphicDev);

private:
    void Free() override;
};
