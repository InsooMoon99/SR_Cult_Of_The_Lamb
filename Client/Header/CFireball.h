#pragma once
#include "CGameObject.h"
#include "CPlayerSkill.h"
#include "CProjectile.h"

namespace Engine
{
class CTransform;
class CTexture;
class CRcTex;
}

class CProjectile;
//#include "CPlayerSkill.h";
struct SkillInfo;

class CFireball : public CGameObject
{

private:
    explicit CFireball(DEVICE graphicDev);
    explicit CFireball(const CFireball& rhs);
    ~CFireball() override;

public:
    void  Initialize_GameObject() override;
    void  LateUpdate_GameObject(const _float& timeDelta) override;
    void  Render_GameObject() override;
    void  OnBeginOverlap(CCollider* self, CCollider* other) override;
    void  OnStayOverlap(CCollider* self, CCollider* other) override;
    void  OnEndOverlap(CCollider* self, CCollider* other) override;
    void  Render_Editor() override;
    HRESULT Add_Component();
    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;

public:
    static CFireball* Create(const CPlayerSkill::SkillInfo& skillInfo, DEVICE graphicDev,CGameObject* instigator, _vec3& skillDir);
    void Create_AttackObj(const CPlayerSkill::SkillInfo& skillInfo, const _vec3& skillDir, CGameObject* instigator);
    Engine::CAttackBase* m_CurrentAttackObj;
protected:
    void Free() override;

private:
    CProjectile* m_ProjectileCom;
    CTransform*  m_TransformCom;
    CTexture*    m_TextureCom;
    CRcTex*      m_BufferCom;
    CGameObject* m_Owner;

   // float m_SpawnOffset;

};


