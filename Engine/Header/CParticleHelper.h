#pragma once

#include "CParticle.h"
#include "CGraphicDev.h"
#include "CCreateHelper.h"

/**
 * @brief  다른 헬퍼는 클래스를 제외하고 static으로 만들었지만 이렇게 클래스로 감싸면 
 *         네임스페이스를 통해서 클래스 이름을 명시해줘야 사용가능하게 할 수도 있어요
 */

class CParticleHelper
{
public:
    template<typename T = CParticle> // 일단 Particle은 자식이 안생길거같아서 고정
    static T* Spawn_Particle(
        const _vec3& position,          // 재생 위치
        const wstring& animKey,         // 스프라이트 키값
        _uint frameCount,               // 몇 프레임?
        _float frameInterval = 0.05f,   // 얼마만큼의 간격으로 재생할것인지
        const _vec3& scale = _vec3(1.f, 1.f, 1.f),  
        const _vec3& offset = _vec3(0.f, 0.f, 0.f),
        COMPONENTTYPE textureType = COMPONENTTYPE::TEX_PARTICLE,
        LAYERTYPE layer = LAYERTYPE::GAMELOGIC,
        bool autoDestroy = true,
        float lifetime = -1.f
    )
    {
        Engine::CGraphicDev* graphicDev = Engine::CGraphicDev::GetInstance();
        NULL_CHECK_RETURN(graphicDev, nullptr);

        DEVICE device = graphicDev->Get_GraphicDev();
        NULL_CHECK_RETURN(device, nullptr);

        T* particle = T::Create(device, position + offset, textureType, animKey);
        NULL_CHECK_RETURN(particle, nullptr);

        particle->Set_AutoDestroy(autoDestroy);
        particle->Set_LifeTime(lifetime);
        particle->Set_ParticleInfo(frameCount, frameInterval);

        auto* transform = dynamic_cast<CTransform*>(
            particle->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (transform)
            transform->Set_Scale(scale);

        particle->Initialize_GameObject();

        SpawnObjectOnLayer(particle, OBJTYPE::PARTICLE);

        return particle;
    }
};

