#include "pch.h"
#include "CBossChain.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CAttackBox.h"


CBossChain::CBossChain(DEVICE graphicDev, CGameObject* owner)
    : CGameObject(graphicDev), m_pOwner(owner)
{
    m_pOwner->AddRef();
}

CBossChain::CBossChain(const CBossChain& rhs) : CGameObject(rhs)
{
}

CBossChain::~CBossChain()
{
}

HRESULT CBossChain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    wstring key = L"BossChainEff";

    // 비율 맞춰주는거 생성할때 외부에서 SetAspectScale로 스케일 조정 
    SetTexAspectInit(m_TextureCom, m_TransformCom, 0 , &key);

    SetAspectScale(m_TransformCom, {.5f,.5f,0.f });

    Animation_Setting();

    m_AnimatorCom->Play_Animation(L"BossChainEff", ANIMSTATE::ONCE);

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    //TODO:: 나중에 땅의 위치를 알게 되면 수정 
    //m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, -ownerTransform->Get_Scale().y * .5f,ownerTransform->Get_Pos().z });
    //m_TransformCom->Set_Pos
    //충돌박스 CAttackBox 생성
    //{
    //    // 1. 공격 박스 생성
    //    auto obj = CAttackBox::Create(m_GraphicDev);
    //    obj->Set_Instigator(m_pOwner); // 공격의 주체를 넣음 - 난 보스를 넣어야 하는듯? 
    //    obj->Set_LifeTime(10.f); // 근데 어짜피 사슬이 사라지면 사라짐 


    //    // 2. 구조체 데이터 세팅
    //    HITDATA hit{};
    //    hit.damage = 1;
    //    hit.knockback = 0.f;
    //    obj->Set_HitDesc(hit);

    //    // 3. 공격 박스 위치 설정, 방향값 추가 가능
    //    _vec3 pos = m_TransformCom->Get_Pos();

    //    auto transform = dynamic_cast<CTransform*>(
    //        obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    //    transform->Set_Pos(pos);

    //    // 4. 레이어에 추가, 헬퍼 함수 SpawnAttackObject 사용
    //    //CManagement::GetInstance()->Get_Scene()->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, obj, OBJTYPE::ATTACK_COL);
    //    SpawnAttackObject(obj); // TYPE , BOX SPHERE
    //}

    return S_OK;
}

_int CBossChain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_flag == false)
    {
        {
            
            // 1. 공격 박스 생성
            ATBox = CAttackBox::Create(m_GraphicDev);
            ATBox->Set_Instigator(m_pOwner); // 공격의 주체를 넣음 - 난 보스를 넣어야 하는듯? 
            ATBox->Set_LifeTime(10.f); // 근데 어짜피 사슬이 사라지면 사라짐 


            // 2. 구조체 데이터 세팅
            HITDATA hit{};
            hit.damage = 1;
            hit.knockback = 0.f;
            ATBox->Set_HitDesc(hit);

            // 3. 공격 박스 위치 설정, 방향값 추가 가능
            _vec3 pos = m_TransformCom->Get_Pos();

            auto transform = dynamic_cast<CTransform*>(
                ATBox->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            transform->Set_Pos(pos);
            transform->Set_Scale(m_TransformCom->Get_Scale());

            // 4. 레이어에 추가, 헬퍼 함수 SpawnAttackObject 사용
            //CManagement::GetInstance()->Get_Scene()->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, obj, OBJTYPE::ATTACK_COL);
            SpawnAttackObject(ATBox); // TYPE , BOX SPHERE
        }
        m_flag = true;
    }

    // 해당 프레임이 끝나면 자기삭제
    if (m_AnimatorCom->Get_CurFrame() == m_AnimatorCom->Get_MaxFrame() - 1)
    {
		this->Mark_As_PendingKill();
    }



    return exit;
}

void CBossChain::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CBossChain::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    //


    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    //
    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();
    //
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


}

void CBossChain::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CBossChain::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

CBossChain* CBossChain::Create(DEVICE graphicDev, CGameObject* owner)
{
    auto BossChain = new CBossChain(graphicDev, owner);

    if (FAILED(BossChain->Ready_GameObject()))
    {
        MSG_BOX("BossChain Create Failed");
        Safe_Release(BossChain);
        return nullptr;
    }

    return BossChain;
}

void CBossChain::Animation_Setting()
{

    m_AnimatorCom->Create_Animation(L"BossChainEff", 16, 0.02f);
    //m_AnimatorCom->Create_Animation(L"ChainGroundEff", 1, 0.02f);

}

HRESULT CBossChain::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BOSSPHASE1);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BOSSPHASE1, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });
    
    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BOSSPHASE1);
    
    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    return S_OK;
}

void CBossChain::Free()
{
   /* Safe_Release(m_pOwner);*/
    CGameObject::Free();
}
