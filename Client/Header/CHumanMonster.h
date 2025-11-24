#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CAnimator;
class CTexture;
class CState;
class CTransform;
class CRcTex;
class CBoxCollider;
class CCombatStat;
END

class CHumanMonster : public CGameObject
{
private:
    explicit CHumanMonster(DEVICE graphicDev);
    explicit CHumanMonster(const CHumanMonster& rhs);
    ~CHumanMonster() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    void    Animation_Setting();
    HRESULT Add_Component();

    void Render_Setting();
    void Render_Reset();

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    CRcTex*       m_BufferCom;
    CTransform*   m_TransformCom;
    CBoxCollider* m_BoxColCom;
    CState*       m_StateCom;
    CTexture*     m_TextureCom;
    CAnimator*    m_AnimatorCom;
    CCombatStat*  m_CombatStatCom;
public:
    static CHumanMonster* Create(DEVICE graphicDev);

private:
    ACTORSTATE m_State;

protected:
    void Free() override;
};
