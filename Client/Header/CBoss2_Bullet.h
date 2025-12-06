#pragma once

#include "Client_Enum.h"
#include "CAttackSphere.h"

class CProjectile;

BEGIN(Engine)

class CTexture;
class CRcTex;

END

class CBoss2_Bullet : public CAttackSphere
{
private:
	explicit CBoss2_Bullet(DEVICE graphicDev);

    explicit CBoss2_Bullet(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE phase);

    explicit CBoss2_Bullet(const CBoss2_Bullet& rhs);
    virtual ~CBoss2_Bullet();

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    void    Start_Shoot() { m_StartShoot = true; }
    void    Set_TargetDir(_vec3 dir) { m_TargetDir = dir; }

protected:
    void    Free() override;

public:
    static CBoss2_Bullet* Create(DEVICE graphicDev);

    static CBoss2_Bullet* Create(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE type);

protected:
    HRESULT Add_Component() override;

private:
    CTexture*    m_TextureCom;
    CTexture*    m_ShadowTexCom; // Shadow Texture
    CProjectile* m_ProjectileCom;
	CRcTex*		 m_BufferCom;

    CGameObject* m_Owner;
    CGameObject* m_Target;

    PROJ_TYPE    m_ProjectileType;

    _vec3        m_StartPos;
    _vec3        m_TargetDir;
    _bool        m_StartShoot = false;

    _float       m_Speed = 15.f;


};

