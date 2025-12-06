#pragma once
#include "CBuildingBase.h"

class CTemple : public CBuildingBase
{
private:
    explicit CTemple(DEVICE graphicDev);
    explicit CTemple(const CTemple& rhs);
    ~CTemple() override;

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
    static CTemple* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTexture* m_TextureCom2 = nullptr;  // 신전 지붕용 텍스처
};
