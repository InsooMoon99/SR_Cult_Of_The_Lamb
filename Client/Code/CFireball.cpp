#include "pch.h"
#include "CFireball.h"
#include "CProjectile.h"
#include "CCreateHelper.h"
#include "CGameObject.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"


CFireball::CFireball(DEVICE graphicDev)
    : CGameObject(graphicDev),
    m_ProjectileCom(nullptr),
    m_TransformCom(nullptr),
    m_Owner(nullptr)
{

}

CFireball::CFireball(const CFireball& rhs)
    : CGameObject(rhs),
    m_ProjectileCom(nullptr),   
    m_TransformCom(nullptr),
    m_Owner(nullptr)
{

}

CFireball::~CFireball()
{

}

HRESULT CFireball::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}


CFireball* CFireball::Create(const CPlayerSkill::SkillInfo& skillInfo, DEVICE graphicDev, CGameObject* instigator, _vec3& skillDir)
{
    auto fireball = new CFireball(graphicDev);

    if (FAILED(fireball->Ready_GameObject()))
    {
        Safe_Release(fireball);
        MSG_BOX("Fireball Create Failed");
        return nullptr;
    }

    fireball->Create_AttackObj(skillInfo, skillDir,instigator);
    
    return fireball;
}

void CFireball::Create_AttackObj(const CPlayerSkill::SkillInfo& skillInfo, const _vec3& skillDir, CGameObject* instigator)
{

    CAttackBuilder(m_GraphicDev, instigator)
        .Box()
        .Track(m_CurrentAttackObj)
        .Damage(dynamic_cast<CCombatStat*>(instigator->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT))->Get_Attack() * skillInfo.damageMultiplier)
        .Knockback(skillInfo.knockbackPower)
        .Scale(skillInfo.hitboxScale)
        .InstigatorPos()
        .Offset(skillDir * skillInfo.spawnOffset)
        .LifeTime(3.f)
        .DestroyOnHit(true)
        .CollisionLayer(COLLISION_LAYERTYPE::PLAYERPROJECTILE)
            .Spawn();


    if (m_CurrentAttackObj)
    {
        m_CurrentAttackObj->Set_OnDestroyCallback([this]()
            {
                this->Mark_As_PendingKill();
            });
    }

    cout << skillDir.x <<"\t" <<skillDir.y <<"\t" <<skillDir.z << endl;

    BulletDesc d;
    d.ProjectileOwner = instigator;
    d.moveType = PROJ_TYPE::LINEAR;
    d.Speed = 10.f;
    d.Direction = skillDir;
    m_ProjectileCom->SetMoveDesc(d);


    m_TransformCom->Set_Pos(instigator->Get_Transform()->Get_Pos());
    m_TransformCom->Set_Scale(0.7f, 0.7f, 0.f);
     m_TransformCom->Set_Rotation(_vec3{ D3DXToRadian(30.f), 0.f, 0.f });
    _vec3 m_SpawnOffset = skillDir * skillInfo.spawnOffset;
}

void CFireball::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();

}

_int CFireball::Update_GameObject(const _float& timeDelta)
{

    int exit = CGameObject::Update_GameObject(timeDelta);
   CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA,this);

    if(m_ProjectileCom)
    m_ProjectileCom->Update_Component(timeDelta);
  // if(m_CurrentAttackObj)
  // m_CurrentAttackObj->Update_GameObject(timeDelta);

    CTransform* projectileTransform = m_ProjectileCom->Get_Transform();

    if (m_CurrentAttackObj)
    {
        CTransform* attackboxTransform = m_CurrentAttackObj->Get_Transform();
        cout << "공격박스 : " << attackboxTransform->Get_Pos().x << "\t" << attackboxTransform->Get_Pos().y << endl;
        m_TransformCom->Set_Pos(projectileTransform->Get_Pos());
        attackboxTransform->Set_Pos(projectileTransform->Get_Pos());
    }
    return exit;
}

void CFireball::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CFireball::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

//    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//
//    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//
//    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
//    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
//    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);


    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

//    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
//    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFireball::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CFireball::OnStayOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnStayOverlap(self, other);
}

void CFireball::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CFireball::Render_Editor()
{
    CGameObject::Render_Editor();
}


HRESULT CFireball::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_SKILL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_SKILL, m_TextureCom });

    // Projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });

    return S_OK;
}

void CFireball::Free()
{
    Safe_Release(m_ProjectileCom);
    Safe_Release(m_TransformCom);
    Safe_Release(m_BufferCom);
    Safe_Release(m_TextureCom);

  //  if (m_CurrentAttackObj)
  //  {
  //      m_CurrentAttackObj->Set_OnDestroyCallback(nullptr);
  //      m_CurrentAttackObj = nullptr;
  //  }

    CGameObject::Free();
}
