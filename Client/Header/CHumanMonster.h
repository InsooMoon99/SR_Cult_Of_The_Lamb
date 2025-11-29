#pragma once

#include "CMonsterBase.h"

BEGIN(Engine)
class CTexture;
END

class CHumanMonster : public CMonsterBase
{
private:
    explicit CHumanMonster(DEVICE graphicDev);
    explicit CHumanMonster(const CHumanMonster& rhs);
    ~CHumanMonster() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    void Chase_Target(const float& timeDelta, _float speed) override;

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    static CHumanMonster* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:

};
