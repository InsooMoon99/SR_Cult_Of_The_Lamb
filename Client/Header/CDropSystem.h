#pragma once
#include "CBase.h"

struct ItemInstance;

BEGIN(Engine)
class CScene;
END

class CInventory;

class CDropSystem : public CBase
{
    DECLARE_SINGLETON(CDropSystem)
private:
    explicit CDropSystem();
    ~CDropSystem() override;

public:
    void SpawnDrop(DEVICE pGraphicDev, const ItemInstance& item, const _vec3& worldPos);
    void SpawnAllFromInven(DEVICE pGraphicDev, CInventory& Inven);

    void SetCurrentScene(CScene* nowScene) { m_nowScene = nowScene; } // 이거 무조건 씬 셋팅 시 해줘야함

private:
    CScene* m_nowScene = nullptr;

private:
    void Free() override;
};
