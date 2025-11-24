#include "CResourceManager.h"

IMPLEMENT_SINGLETON(CResourceManager)

CResourceManager::CResourceManager()
{
}

CResourceManager::~CResourceManager()
{
}

HRESULT CResourceManager::Add_Sprite(DEVICE dev, const wstring& key, const wstring& path,
    _uint frameCnt, _uint startOffset)
{
    auto& frames = m_SpriteMap[key];
    frames.reserve(frameCnt);

    IDirect3DBaseTexture9* tex = nullptr;

    for (_uint i = 0; i < frameCnt; ++i)
    {
        TCHAR fileName[256] = L"";
        wsprintf(fileName, path.c_str(), i + startOffset);

        if (FAILED(D3DXCreateTextureFromFileEx(
            dev,
            fileName,
            D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
            1, 0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT,
            0, NULL, NULL,
            (LPDIRECT3DTEXTURE9*)&tex)))
            return E_FAIL;

        frames.push_back(tex);
    }

    return S_OK;
}

HRESULT CResourceManager::Add_Texture(LPDIRECT3DDEVICE9 dev, const wstring& key, const wstring& filePath)
{
    return Add_Sprite(dev, key, filePath, 1, 0);
}

const vector<IDirect3DBaseTexture9*>& CResourceManager::Get_Sprite(const wstring& key)
{
    auto& v = m_SpriteMap[key];
    for (auto* t : v)
        if (t) t->AddRef();
    return v;
}

void CResourceManager::Free()
{
    for (auto& kv : m_SpriteMap)
        for (auto* t : kv.second)
            Safe_Release(t);

    m_SpriteMap.clear();
}
