#include "pch.h"
#include "CBoss2_Bullet.h"

CBoss2_Bullet::CBoss2_Bullet(DEVICE graphicDev, CGameObject* owner, CGameObject* target, PROJ_TYPE phase)
{
}

CBoss2_Bullet::CBoss2_Bullet(const CBoss2_Bullet& rhs)
{
}

CBoss2_Bullet::~CBoss2_Bullet()
{
}

CBoss2_Bullet* CBoss2_Bullet::Create(DEVICE graphicDev, CGameObject* owner, CGameObject* target, PROJ_TYPE type)
{
}

void CBoss2_Bullet::Free()
{
    CRenderObject::Free();

}
