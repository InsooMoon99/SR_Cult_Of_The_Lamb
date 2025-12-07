#pragma once
#include "CBase.h"
#include "Client_Struct.h"
#include <optional>

struct ItemInstance;

BEGIN(Engine)
class CScene;
END

class CInventory;
class CDroppedItem;

class CDropSystem : public CBase
{
    DECLARE_SINGLETON(CDropSystem)
private:
    explicit CDropSystem();
    ~CDropSystem() override;

public:

    void SpawnDrop(DEVICE pGraphicDev, const ItemInstance& item, const _vec3& worldPos, std::optional<SpriteBounceDesc> bounceDesc = std::nullopt);
    void SpawnAllFromInven(DEVICE pGraphicDev, CInventory* Inven, _vec3 dropDir = { 0,0,0 }, int offset = 0);

    void Clear_Item();

    void SetCurrentScene(CScene* nowScene) { m_nowScene = nowScene; } // 이거 무조건 씬 셋팅 시 해줘야함

private:
    CScene* m_nowScene = nullptr;
    bool isBouncing = false;


    list<CDroppedItem*> tmpItem;
private:
    void Free() override;
};
