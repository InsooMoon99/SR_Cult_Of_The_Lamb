#pragma once

#include "CAttackBase.h"
#include "CAttackBox.h"
#include "CAttackSphere.h"
#include "CCreateHelper.h"
#include "Engine_Define.h"
#include "CTransform.h"

BEGIN(Engine)

/// @brief Attack 객체 생성을 위한 빌더 클래스
class ENGINE_DLL CAttackBuilder
{
public:
    CAttackBuilder(DEVICE graphicDev, CGameObject* instigator)
        : m_GraphicDev(graphicDev)
          , m_Instigator(instigator)
          , m_AttackObj(nullptr)
          , m_LifeTime(0.4f), m_Position(0.f, 0.f, 0.f)
          , m_Offset(0.f, 0.f, 0.f)
          , m_Scale(0.f, 0.f, 0.f)
          , m_UseInstigatorPos(false), m_DestroyOnHit(false)
    {
    }

    // 공격 타입 ex) Box, Sphere
    CAttackBuilder& Box()
    {
        m_AttackObj = CAttackBox::Create(m_GraphicDev);
        return *this;
    }

    CAttackBuilder& Sphere()
    {
        m_AttackObj = CAttackSphere::Create(m_GraphicDev);
        return *this;
    }

    CAttackBuilder& Damage(_float damage)
    {
        m_AttackData.damage = damage;
        return *this;
    }

    CAttackBuilder& Knockback(_float knockback)
    {
        m_AttackData.knockback = knockback;
        return *this;
    }

    CAttackBuilder& Duration(_float duration)
    {
        m_AttackData.duration = duration;
        return *this;
    }

    CAttackBuilder& LifeTime(_float lifeTime)
    {
        m_LifeTime = lifeTime;
        return *this;
    }

    // 절대 위치 (내가 원하는 위치)
    CAttackBuilder& Position(const _vec3& pos)
    {
        m_Position = pos;
        m_UseInstigatorPos = false;
        return *this;
    }

    // 공격 주체자의 위치
    CAttackBuilder& InstigatorPos()
    {
        m_UseInstigatorPos = true;
        return *this;
    }

    // 소환 위치 조정
    CAttackBuilder& Offset(const _vec3& offset)
    {
        m_Offset = offset;
        return *this;
    }

    // 앞으로
    CAttackBuilder& Forward(float distance)
    {
        m_Offset.z += distance;
        return *this;
    }
    // 위로
    CAttackBuilder& Up(float distance)
    {
        m_Offset.y += distance;
        return *this;
    }

    CAttackBuilder& Scale(const _vec3& scale)
    {
        m_Scale = scale;
        return *this;
    }

    // 공격 후 삭제할지?
    CAttackBuilder& DestroyOnHit(bool destroy = true)
    {
        m_DestroyOnHit = destroy;
        return *this;
    }

    CAttackBuilder& Scale(_float scale)
    {
        m_Scale = _vec3(scale, scale, scale);
        return *this;
    }

    void Spawn()
    {
        NULL_CHECK(m_AttackObj);

        // 공격한놈 설정
        m_AttackObj->Set_Instigator(m_Instigator);

        // AttackData 설정 
        m_AttackObj->Set_AttackData(m_AttackData);

        // LiteTime 설정
        m_AttackObj->Set_LifeTime(m_LifeTime);

        // Destroy
        m_AttackObj->Set_DestroyOnHit(m_DestroyOnHit);

        // Transform 설정
        auto transform = dynamic_cast<CTransform*>(
            m_AttackObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (transform)
        {
            // 위치 계산
            _vec3 finalPos = m_Position;

            if (m_UseInstigatorPos && m_Instigator)
            {
                auto instigatorTrans = dynamic_cast<CTransform*>(
                    m_Instigator->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (instigatorTrans)
                    finalPos = instigatorTrans->Get_Pos();
            }

            finalPos += m_Offset;
            transform->Set_Pos(finalPos);

            // 스케일 설정
            if (m_Scale.x > 0.f)
                transform->Set_Scale(m_Scale);

            // Rotation은 알아서
        }

        // GameLogic Layer에 추가
        SpawnAttackObject(m_AttackObj);

        // 정리
        m_AttackObj = nullptr;
    }


private:
    DEVICE              m_GraphicDev;
    CGameObject*        m_Instigator;
    CAttackBase*        m_AttackObj;

    ATTACKDATA          m_AttackData;  
    _float              m_LifeTime;

    _vec3               m_Position;
    _vec3               m_Offset;
    _vec3               m_Scale;
    _bool               m_UseInstigatorPos; // 공격자의 위치를 사용할지
    _bool               m_DestroyOnHit;
};

END
