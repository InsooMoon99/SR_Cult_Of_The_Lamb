#include "CFont.h"

CFont::CFont(DEVICE graphicDev)
    : m_GraphicDev(graphicDev), m_Sprite(nullptr), m_Font(nullptr)
{
    m_GraphicDev->AddRef();
}

CFont::~CFont()
{ }

HRESULT CFont::Ready_Font(const wstring& fontType,
                          const _uint&   width,
                          const _uint&   height,
                          const _uint&   weight)
{
    D3DXFONT_DESC fontDesc{};

    fontDesc.CharSet = HANGUL_CHARSET;
    fontDesc.Width   = width;
    fontDesc.Height  = static_cast<_int>(height);
    fontDesc.Weight  = weight;
    lstrcpy(fontDesc.FaceName, fontType.c_str());

    if (FAILED(D3DXCreateFontIndirect(m_GraphicDev, &fontDesc, &m_Font)))
    {
        MSG_BOX("Font Create Failed");
        return E_FAIL;
    }

    if (FAILED(D3DXCreateSprite(m_GraphicDev, &m_Sprite)))
    {
        MSG_BOX("Sprite Create Failed");
        return E_FAIL;
    }

    return S_OK;
}

void CFont::Render_Font(const wstring& text, const _vec2* pos, D3DXCOLOR color)
{
    //RECT rc{ static_cast<_long>(pos->x), static_cast<_long>(pos->y), 0, 0 };

    //m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

    //m_Font->DrawTextW(m_Sprite, text.c_str(), static_cast<_int>(text.length()), &rc, DT_NOCLIP, color);
    //// DT_NOCLIP : 사각형 영역에 의한 클리핑 없이 출력 (정렬 옵션 없음)
    //// 다른 정렬 옵션을 사용할경우 rc 값을
    //// 전부 채워 RECT 전체를 구성할 것

    //m_Sprite->End();


    RECT rc;
    rc.left = static_cast<LONG>(pos->x - 500);
    rc.top = static_cast<LONG>(pos->y - 200);
    rc.right = static_cast<LONG>(pos->x + 500);
    rc.bottom = static_cast<LONG>(pos->y + 200);

    m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

    m_Font->DrawTextW(
        m_Sprite,
        text.c_str(),
        static_cast<int>(text.length()),
        &rc,
        DT_CENTER | DT_VCENTER,     // 중앙기준
        color
    );

    m_Sprite->End();
}

CFont* CFont::Create(DEVICE         graphicDev,
                     const wstring& fontType,
                     const _uint&   width,
                     const _uint&   height,
                     const _uint&   weight)
{
    auto font = new CFont(graphicDev);

    if (FAILED(font->Ready_Font(fontType, width, height, weight)))
    {
        Safe_Release(font);
        MSG_BOX("Font Create Failed");
        return nullptr;
    }

    return font;
}

void CFont::Free()
{
    Safe_Release(m_Font);
    Safe_Release(m_Sprite);
    Safe_Release(m_GraphicDev);
}
