#include "pch.h"
#include "CRenderer.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CState.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CCombatStat.h"
#include "CMonsterState.h"



CBossPhase1::CBossPhase1(DEVICE graphicDev)
    : CMonsterBase(graphicDev)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::CBossPhase1(const CBossPhase1& rhs)
    : CMonsterBase(rhs)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::~CBossPhase1()
{
}

HRESULT CBossPhase1::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_StateMachine = CBossStateMachine::Create(this);

    m_TransformCom->Set_Scale({ 5,5,1 });

    m_BoxColCom->Set_Size({ m_TransformCom->Get_Scale().x*.2f,m_TransformCom->Get_Scale().y ,m_TransformCom->Get_Scale().z*.2f });

    BaseCenter.y = m_TransformCom->Get_Scale().y;
    m_TransformCom->Set_Pos(StartPos.x, m_TransformCom->Get_Scale().y, StartPos.z);

    m_CombatStatCom->Set_Hp(200.f);
    m_CombatStatCom->Set_MaxHp(200.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);


    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ENEMY);

    return S_OK;
}

void CBossPhase1::Initialize_GameObject()
{
   
    BaseCenter = { 20 ,m_TransformCom->Get_Scale().y, 20 };
    m_TransformCom->Set_Pos(StartPos.x, m_TransformCom->Get_Scale().y, StartPos.z);

}
void CBossPhase1::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

    m_bHurt = true;

}
void CBossPhase1::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}
_int CBossPhase1::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    m_StateMachine->Update_StateMachine(timeDelta);

    if (m_bHurt == true)
    {
        m_bHurtTime += timeDelta;
        if (m_bHurtTime > 0.1f)
        {
            m_bHurt = false;
            m_bHurtTime = 0;
        }
    }

    return exit;
}

void CBossPhase1::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
    m_StateMachine->LateUpdate_StateMachine(timeDelta);
}

void CBossPhase1::Render_GameObject()
{
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    m_StateMachine->Get_CurState()->Render_State();

    // 4) 텍스처 바인딩
    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        int frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(key, frame);
    }

    //  5) 기본 COLOROP = MODULATE (절대 ADD 쓰면 안됨)
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    //  6) Hurt 상태면 색 덮어쓰기
    if (m_bHurt)
    {
        // 붉게 번쩍이게 설정
        m_GraphicDev->SetRenderState(
            D3DRS_TEXTUREFACTOR,
            D3DCOLOR_ARGB(255, 255, 255, 255) // ← 빨간 하이라이트
        );

        // TEXTURE * TFACTOR 로 변경하면 Tint 색 정확히 나옴
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

        //알파
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)(m_fAlpha * 255), 255, 255, 255));
        //
        m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용

    }

    // 7) 버텍스 버퍼 렌더
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    // 8) RenderState Reset
    Render_Reset();

    // Hurt tint가 남지 않게 기본 연산도 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // 알파 리셋
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로

    // 9) Debug Collider 렌더 (영향 안 받음, OK)
    if (m_BoxColCom)
        m_BoxColCom->Render();
   
}

void CBossPhase1::Render_Editor()
{
    ImGui::Text("=== BossP1 ===");

    _int num = m_StateMachine->GetCurStateNum();

    for (int i = 0; i < BOSS_END; i++)
    {
        if (ImGui::Button(GetBossStateName(i)))
        {
            m_StateMachine->Change_State(i);
        }
    }
}


HRESULT CBossPhase1::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BOSSPHASE1 );
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BOSSPHASE1, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BOSSPHASE1);

    return S_OK;
}

void CBossPhase1::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"BossIdle", 100, 0.02f);

    m_AnimatorCom->Create_Animation(L"BossAwake", 105, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossAttack", 97, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossDead", 167, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_up", 50, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_Down", 28, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_start", 115, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_loop", 84, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossGroggy_end", 67, 0.01f);

    m_AnimatorCom->Create_Animation(L"BossChainEff", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"ChainGroundEff", 1, 0.02f);

    m_AnimatorCom->Create_Animation(L"Before_Idle", 334, 0.02f);
    m_AnimatorCom->Create_Animation(L"Escape",119, 0.02f);

}

const char* CBossPhase1::GetBossStateName(_int StateNum)
{

    switch (StateNum)
    {
    case BOSSAWAKE:         return "BOSSAWAKE";
    case BULLETATTACK:      return "BULLETATTACK";
    case CHAINATTACK:       return "CHAINATTACK";
    case BOSSDEAD:          return "BOSSDEAD";
    case BOSSIDLE:          return "BOSSIDLE";
    case BOSSMOVE:          return "BOSSMOVE";
    case BOSSGROGGRY:       return "BOSSGROGGRY";
    case TALK:              return "TALK";
    case BOSS_END:          return "BOSS_END";
    }
    return "UNKNOWN";
}

CBossPhase1* CBossPhase1::Create(DEVICE graphicDev)
{
    auto Boss = new CBossPhase1(graphicDev);

    if (FAILED(Boss->Ready_GameObject()))
    {
        MSG_BOX("Boss Create Failed");
        Safe_Release(Boss);
        return nullptr;
    }

    return Boss;
}

void CBossPhase1::Free()
{
    //m_StateMachine->Release();
    Safe_Release(m_StateMachine);

    CMonsterBase::Free();

}
