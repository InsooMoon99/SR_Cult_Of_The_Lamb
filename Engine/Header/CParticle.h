#pragma once

#include "CRenderObject.h"

BEGIN(Engine)

class CAnimator;

class ENGINE_DLL CParticle : public CRenderObject
{
private:
    explicit CParticle(DEVICE graphicDev);
    explicit CParticle(const CParticle& rhs);
    virtual ~CParticle() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    HRESULT Add_Component() override;
    void    Particle_Settings();

public:
    void Set_AutoDestroy(bool value) { m_AutoDestroy = value; }
    void Set_LifeTime(float time) { m_LifeTime = time; }
    void Set_TextureType(COMPONENTTYPE type) { m_TextureType = type; }
    void Set_AnimKey(const wstring& key) { m_AnimKey = key; }
    void Set_ParticleInfo(_int frameCount, _float interval);

    static CParticle* Create(DEVICE graphicDev,
                                const _vec3& position,
                                COMPONENTTYPE textureType,
                                const wstring& animKey);

protected:
    void Free() override;

private:
    CAnimator*      m_AnimatorCom;

    bool            m_AutoDestroy;   // 재생이 끝나면 삭제되게 할지?
    float           m_LifeTime;      // 생명 시간 (-1이면 무한)
    float           m_ElapsedTime;   // 경과 시간

    COMPONENTTYPE   m_TextureType;
    wstring         m_AnimKey;

    _uint           m_FrameCount;
    _float          m_FrameInterval;
};

END
