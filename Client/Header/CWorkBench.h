#pragma once
#include "CBuildingBase.h"

class CWorkBench : public CBuildingBase
{
private:
    explicit CWorkBench(DEVICE graphicDev);
    explicit CWorkBench(const CWorkBench& rhs);
    ~CWorkBench() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void Render_Editor() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component() override;

public:
    static CWorkBench* Create(DEVICE graphicDev);

private:
    void Free() override;
};
