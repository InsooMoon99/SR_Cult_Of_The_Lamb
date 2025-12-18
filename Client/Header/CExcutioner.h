#pragma once

#include "CMonsterBase.h"

class CUI_MonsterHp;

BEGIN(Engine)
class CTexture;
END

class CExcutioner : public CMonsterBase
{
private:
    explicit CExcutioner(DEVICE graphicDev);
    explicit CExcutioner(const CExcutioner& rhs);
    ~CExcutioner() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    // 상태 변환
    void    Update_State_Idle(const _float timeDelta);
    void    Update_State_Attack(const _float timeDelta);

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    static CExcutioner* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CUI_MonsterHp*  m_HpBar;

};
