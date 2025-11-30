#pragma once

#include "CBase.h"
#include "CMapObjectBase.h"

/**
 * @brief 에디터 전용 맵 오브젝트 배치 헬퍼
 * CMainEditor에서 사용 하는 클래스임
 */
class CMapEditorPlacer : public CBase
{
private:
    explicit CMapEditorPlacer(DEVICE graphicDev);
    explicit CMapEditorPlacer(const CMapEditorPlacer& rhs);
    ~CMapEditorPlacer();

public:
    void        Update_MapPlacer(const _float& timeDelta);
    void        Render_Privew();

    void        Start_Place(MAPOBJTYPE type);  // 배치 시작
    void        Cancel_Place();                // 배치 취소

    _bool       IsPlacing() { return m_Placing; }
    _bool       Get_MouseWorldPos(_vec3& outPos);
    MAPOBJTYPE  Get_CurType() { return m_SelectedType; }

    _int        Get_CurTextureIndex();
    _float      Get_PreviewScale() { return m_PreviewScale; }
    _vec3       Get_PreviewRotation() { return m_PreviewRotation; }

    void        Set_GrassIndex(_int index) { m_GrassIndex = index; }
    void        Set_RubbleIndex(_int index) { m_RubbleIndex = index; }
    void        Set_DecoIndex(_int index) { m_DecoIndex = index; }
    void        Set_DungeonDecoIndex(_int index) { m_DungeonDecoIndex = index; }
    void        Set_ETCIndex(_int index) { m_ETCIndex = index; }

public:
    static CMapEditorPlacer* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    DEVICE          m_GraphicDev;
    CMapObjectBase* m_PreviewObj;
    MAPOBJTYPE      m_SelectedType;
    _bool           m_Placing;

    _int            m_GrassIndex;
    _int            m_RubbleIndex;
    _int            m_DecoIndex;
    _int            m_DungeonDecoIndex;
    _int            m_ETCIndex;

    _float          m_PreviewScale;
    _vec3           m_PreviewRotation;
    _float          m_PreviewYOffset;

};

