
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
class CState;
END

class CEtc :
    public CGameObject
{
private:
    explicit CEtc(DEVICE graphicDev, COMPONENTTYPE _TexComType,
        wstring _AnimationKey,
        int _SpriteNum);
    explicit CEtc(const CEtc& rhs);
    ~CEtc() override;

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
    CRcTex* m_BufferCom;
    CTransform* m_TransformCom;
    CTexture* m_TextureCom;
    CAnimator* m_AnimatorCom;
    CState* m_StateCom;

    COMPONENTTYPE m_TexComType;
    wstring m_AnimationKey;
    int m_SpriteNum;

public:
    static CEtc* Create(DEVICE graphicDev, COMPONENTTYPE _TexComType,
    wstring _AnimationKey,
    int _SpriteNum);

private:
    void Free() override;
};
