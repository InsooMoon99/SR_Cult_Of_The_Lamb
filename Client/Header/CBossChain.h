#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CBoxCollider;
class CCombatStat;
END

class CBossChain :
    public CGameObject
{

private:
    explicit CBossChain(DEVICE graphicDev, CGameObject* owner);
    explicit CBossChain(const CBossChain& rhs);
    virtual ~CBossChain();

public:

    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

    void Animation_Setting();
    HRESULT Add_Component();
public:

    static CBossChain* Create(DEVICE graphicDev, CGameObject* owner);

protected:

    CRcTex* m_BufferCom;
    CTransform* m_TransformCom;
    CTexture* m_TextureCom;
    CAnimator* m_AnimatorCom;
    CBoxCollider* m_BoxColCom;
    CCombatStat* m_CombatStatCom;

    CGameObject* m_pOwner;
private:
    void Free() override;



};

