#include "pch.h"
#include "CSpriteEff.h"
#include "CGameObject.h"
#include "CTransform.h"

CSpriteEff::CSpriteEff()
{
}

CSpriteEff::CSpriteEff(DEVICE graphicDev) : CEffectBaseCom(graphicDev)
, is_SetSpriteDesc(false)
{
}

CSpriteEff::CSpriteEff(const CSpriteEff& rhs) : CEffectBaseCom(rhs)

{
    //컴포넌트는 현구조상 프로토 타입으로 클론하기 떄문에 깊은 복사 , 초기화 필수

}

CSpriteEff::~CSpriteEff()
{
}

HRESULT CSpriteEff::Ready_Effect()
{
    return S_OK;
}

void CSpriteEff::Initialize_Component()
{
}

_int CSpriteEff::Update_Component(const _float& timeDelta)
{



    if (m_IsSetBouncingInit)
    {
        if (!m_Owner)
            return -1;

        // 가속도 -> 속도
        m_Velocity.y += m_Gravity * timeDelta;

        //속도 -> 위치
        CTransform* ownerTransform = m_Owner->Get_Transform();
        ownerTransform->Set_Pos(ownerTransform->Get_Pos() + m_Velocity * timeDelta);

        if (ownerTransform->Get_Pos().y <= m_GroundY) // 바닥 충돌 처리
        {
            // ... (위치 강제 설정, Y축 튕김 계산, X/Z에 m_Damping 곱하기) ...
            ownerTransform->Set_Pos({ ownerTransform->Get_Pos().x, m_GroundY, ownerTransform->Get_Pos().z }); // 위치 보정

            m_Velocity.y = -m_Velocity.y * m_Restitution; // Y축 튕김 계산

            // 📌 충돌 직후 1회성 X/Z 감쇠 (이 코드가 누락되면 미끄러짐이 심해집니다)
            m_Velocity.x *= m_Damping;
            m_Velocity.z *= m_Damping;

            if (fabsf(m_Velocity.y) < m_STOP_VEL)
            {
                m_Velocity.y = 0.f;

                // **추가 로직:** Y축이 멈춘 상태(바닥에 붙은 상태)일 때, X/Z 속도에 지속적인 감쇠(마찰)를 적용합니다.
                // m_Damping 값을 사용하여 바닥 마찰을 흉내냅니다.
                m_Velocity.x *= powf(m_Damping, timeDelta * 10.f); // timeDelta에 10 정도를 곱해 감쇠 속도를 높여도 좋습니다.
                m_Velocity.z *= powf(m_Damping, timeDelta * 10.f);

                // 마지막 미세 떨림 방지 (X/Z가 0이 되는 최종 조건)
                if (fabsf(m_Velocity.x) < 0.5f && fabsf(m_Velocity.z) < 0.5f)
                {
                    m_Velocity.x = 0.f;
                    m_Velocity.z = 0.f;
                    // ... (Update 종료 로직) ...
                    m_IsSetBouncingInit = false;
                    return 0;
                }
            }
        }

        // ... (이 부분은 공중에 있을 때의 공기 저항으로 유지) ...
        float factor = powf(m_DampingPerSecond, timeDelta);
        m_Velocity.x *= factor;
        m_Velocity.z *= factor;

        return 0;

    }

    
    return 0;
}

void CSpriteEff::LateUpdate_Component()
{
}

void CSpriteEff::Render_Effect()
{
}

CSpriteEff* CSpriteEff::Create(DEVICE graphicDev)
{
    auto SpriteEff = new CSpriteEff(graphicDev);

    if (FAILED(SpriteEff->Ready_Effect()))
    {
        MSG_BOX("SpriteEff Create Failed");
        Safe_Release(SpriteEff);
        return nullptr;
    }

    return SpriteEff;
}

CComponent* CSpriteEff::Clone()
{
    return new CSpriteEff(*this);
}

void CSpriteEff::Free()
{
    CEffectBaseCom::Free();
}
