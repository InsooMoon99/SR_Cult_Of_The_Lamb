#include "pch.h"
#include "CEligos_Flies.h"


#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CParticleHelper.h"
#include "CPlayer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"
#include "CProjectile.h"
#include "CSoundMgr.h"

CEligos_Flies::CEligos_Flies(DEVICE graphicDev, CGameObject* owner, CGameObject* Target)
    : CMonsterBase(graphicDev), m_pOwner(owner), m_pTarget(Target)
{
}

CEligos_Flies::CEligos_Flies(const CEligos_Flies& rhs) : CMonsterBase(rhs)
{
}

CEligos_Flies::~CEligos_Flies()
{
}

HRESULT CEligos_Flies::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));


    m_CombatStatCom->Set_MaxHp(1.f);
    m_CombatStatCom->Set_Attack(1.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);
    m_CombatStatCom->Set_HitState(false);


    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE);


    return S_OK;
}

void CEligos_Flies::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            LAYERTYPE::GAMELOGIC,
            OBJTYPE::PLAYER,
            COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();

    BulletDesc d;
    d.Radius = 0;
    d.OrbitCenter = { 0,1,0 };
    d.StartAngle = 0;
    d.Speed = 1.5f;
    d.moveType = PROJ_TYPE::HOMING;
    d.ProjectileOwner = m_pOwner;
    d.Target = m_pTarget;
    d.TurnSpeed = d.Speed * 2.f;;

    m_ProjectileCom->SetMoveDesc(d);
}

_int CEligos_Flies::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (m_spawnSound == false)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Fly_Spawn.wav", SOUNDTYPE::SOUND_BOSS4, 1.0f);
       
        m_spawnSound = true;
    }
#pragma region 플레이어 베리 채집 효과음. 0.5초마다 재생
    g_Timers[TIMER_SOUND_ENEMY_FLY] -= timeDelta;

    if (g_Timers[TIMER_SOUND_ENEMY_FLY] < 0.f)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Fly_Fly_Loop.wav", SOUND_BOSS2, 0.5f);
        g_Timers[TIMER_SOUND_ENEMY_FLY] = 0.5f;
    }
#pragma endregion
    m_ProjectileCom->Update_Component(timeDelta);
    
    if (m_CombatStatCom->Is_Dead() == true||m_flyDie==true)
    {
        //죽었을떄 애니메이션 추가
        //끝나면
        CSoundMgr::GetInstance()->Stop_Sound(L"Boss/Eligos/Fly_Fly_Loop.wav");
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Flying_Hopper_Miniboss_Bite.wav", SOUNDTYPE::SOUND_BOSS3, 0.7f);

        Mark_As_PendingKill();
    }

    if (m_flag == false)
    {
        m_AnimatorCom->Play_Animation(L"Mortar_fly", ANIMSTATE::LOOP);
        m_flag = true;
    }


    m_Timer += timeDelta;
    if (m_Timer > 600)
    {
        Mark_As_PendingKill();
    }
    return exit;
}

void CEligos_Flies::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CEligos_Flies::Render_GameObject()
{


    CMonsterBase::Render_GameObject();


    //if (m_TransformCom)
    //    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //Render_Setting();

    //// 4) 텍스처 바인딩
    //if (m_TextureCom && m_AnimatorCom)
    //{
    //    const wstring& key = m_AnimatorCom->Get_CurKey();
    //    int            frame = m_AnimatorCom->Get_CurFrame();
    //    m_TextureCom->Set_Texture(key, frame);
    //}

    ////  5) 기본 초기화 (선택사항)
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    //if (m_bHurt)
    //{
    //    // 현제 값은 초기 값세팅을 한거 나중에 변경 하고 싶으면 인자 넣으면 됩니다 색,투명도
    //    FlashRenderStart();//색 (ARGB)
    //    AlphaRenderStart();//투명도(0~1)
    //}

    //// 7) 버텍스 버퍼 렌더
    //if (m_BufferCom)
    //    m_BufferCom->Render_Buffer();

    //// 초기화
    //if (m_bHurt)
    //{
    //    FlashRenderEnd();
    //    AlphaRenderEnd();
    //}
    //if (m_BoxColCom)
    //    m_BoxColCom->Render();
}


void CEligos_Flies::Update_Direction()
{
}

void CEligos_Flies::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Mortar_fly", 27, 0.02f);
}

void CEligos_Flies::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

    CPlayer* player = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (player)
    {
        CCombatStat* Player_combat = dynamic_cast<CCombatStat*>(player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        CTransform* Player_tans = dynamic_cast<CTransform*>(player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        _vec3 tmp = Player_tans->Get_Pos() - m_TransformCom->Get_Pos();

        _vec3 dir;
            D3DXVec3Normalize(&dir, &tmp);

        HITDATA hit;
        hit.  dir = dir;              // 넉백 방향
        hit. damage = m_CombatStatCom->Get_Attack();           // 데미지
        hit. knockback=2.f;        // 넉백량
        hit. knockbackTimer=0.5f;         // 넉백 유지 시간

        hit. attacker = this;   // 때린 놈
        hit. hitter = player;     // 맞은 놈


        Player_combat->Apply_Damage(hit);

        // 나 죽을 수 있게 
        m_flyDie =true;
    }
}

void CEligos_Flies::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}



HRESULT CEligos_Flies::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_FLY);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_FLY, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_FLY);

    // projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });
    return S_OK;
}

CEligos_Flies* CEligos_Flies::Create(DEVICE graphicDev, CGameObject* owner, CGameObject* Target)
{
    auto Eligos_Flies = new CEligos_Flies(graphicDev, owner, Target);

    if (FAILED(Eligos_Flies->Ready_GameObject()))
    {
        MSG_BOX("Eligos_Flies Create Failed");
        Safe_Release(Eligos_Flies);
        return nullptr;
    }

    return Eligos_Flies;
}

void CEligos_Flies::Free()
{
    CMonsterBase::Free();
}
