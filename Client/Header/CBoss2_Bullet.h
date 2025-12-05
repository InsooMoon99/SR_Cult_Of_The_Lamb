#pragma once

#include "Client_Enum.h"
#include "CRenderObject.h"

class CProjectile;

class CBoss2_Bullet : public CRenderObject
{
private:
    explicit CBoss2_Bullet(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE phase);

    explicit CBoss2_Bullet(const CBoss2_Bullet& rhs);
    virtual ~CBoss2_Bullet();

public:
    void Start_Shoot() { m_StartShoot = true; }
    void Set_TargetDir(_vec3 dir) { m_TargetDir = dir; }

protected:
    void Free() override;

public:
    static CBoss2_Bullet* Create(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE type);

private:
    CGameObject* m_pOwner;
    CGameObject* m_pTarget;
    CProjectile* m_ProjectileCom;

    PROJ_TYPE    m_type;

    _vec3        m_StartPos;
    _vec3        m_TargetDir;
    _bool        m_StartShoot = false;

    _float       m_Speed = 15.f;


};

