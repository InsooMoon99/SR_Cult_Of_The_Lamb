#pragma once

#include "CCharacterBase.h"
#include "IHitFlashEff.h"

BEGIN(Engine)
class CTransform;
END

// 모든 몬스터들의 기본 베이스 (HumanMonster, Boss ..)
class CMonsterBase : public CCharacterBase, public IHitFlashEff
{
protected:
    explicit CMonsterBase(DEVICE graphicDev);
    explicit CMonsterBase(const CMonsterBase& rhs);
    virtual ~CMonsterBase();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
public:
    _float Get_DistanceToTarget();

public:
    // 충돌 처리
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    // 상속받아서 몬스터마다 다르게 사용 ㄱㄱ
    virtual void Chase_Target(const float& timeDelta, _float speed) {}

    // IHitFlashEff 구현부
    virtual void FlashRenderStart(DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255));
    virtual void FlashRenderEnd() ;
    virtual void AlphaRenderStart(float alpha = 0.8f) ;
    virtual void AlphaRenderEnd(); 

    bool m_bHurt = false;
    float HurtTime = 0;
    float flashTime = 0.1f;
protected:
    void Free() override;

protected:
    CGameObject* m_Target;
    CTransform*  m_TargetTransform;

};

