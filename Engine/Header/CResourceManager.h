#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CResourceManager
{
    DECLARE_SINGLETON(CResourceManager)

private:
    CResourceManager();
    ~CResourceManager();

public:
    // Sprite(프레임 시퀀스) 로드
    HRESULT Add_Sprite(DEVICE dev,
        const wstring& key,
        const wstring& path, // ...%d.png
        _uint frameCnt,
        _uint startOffset = 0);

    // 단일 텍스처 (프레임 1개짜리)도 Sprite 취급
    HRESULT Add_Texture(DEVICE dev,
        const wstring& key,
        const wstring& filePath);

    // key 에 해당하는 프레임 벡터 반환 (AddRef 포함)
    const vector<IDirect3DBaseTexture9*>& Get_Sprite(const wstring& key);

private:
    void Free();

private:
    // key → [frame0, frame1, ...]
    unordered_map<wstring, vector<IDirect3DBaseTexture9*>> m_SpriteMap;
};
;
END
