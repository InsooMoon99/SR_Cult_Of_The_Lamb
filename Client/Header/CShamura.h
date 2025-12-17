#pragma once

#include "CMonsterBase.h"

class CUI_MonsterHp;

BEGIN(Engine)
class CTexture;
END

class CShamura : public CMonsterBase
{
private:
    explicit CShamura(DEVICE graphicDev);
    explicit CShamura(const CShamura& rhs);
    ~CShamura() override;

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
    static CShamura* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CUI_MonsterHp*  m_HpBar;

};
