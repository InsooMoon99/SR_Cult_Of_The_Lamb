#pragma once

#include "CMonsterBase.h"

class CUI_MonsterHp;

BEGIN(Engine)
class CTexture;
END

class CKallamar : public CMonsterBase
{
private:
    explicit CKallamar(DEVICE graphicDev);
    explicit CKallamar(const CKallamar& rhs);
    ~CKallamar() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    static CKallamar* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CUI_MonsterHp*  m_HpBar;

};
