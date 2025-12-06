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
    //TODO 신전 지붕은, 신전 앞부분과 어느 정도 거리를 둔 채(오프셋) 위치해야 하는데, 이는 어떻게 해결할 것인가?
};
