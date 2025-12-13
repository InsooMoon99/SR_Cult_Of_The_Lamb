#include "pch.h"
#include "CTrailEff.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CManagement.h"
#include "CScene.h"
#include "CMainCamera.h"

CTrailEff::CTrailEff()
{
}

CTrailEff::CTrailEff(DEVICE graphicDev) : CEffectBaseCom(graphicDev)
{
}

CTrailEff::CTrailEff(const CTrailEff& rhs) : CEffectBaseCom(rhs)
{
}

CTrailEff::~CTrailEff()
{
}

HRESULT CTrailEff::Ready_Effect()
{
    return S_OK;
}

void CTrailEff::Initialize_Component()
{
}

_int CTrailEff::Update_Component(const _float& timeDelta)
{
    _vec3 curPos = m_TransformCom->Get_Pos();

    _vec3 tmp = curPos - m_LastPos;
    float dist = D3DXVec3Length(&tmp);


    if (m_Node.empty() || dist >= m_NodeIntervalDist)
    {
        TrailNode node;
        node.pos = curPos;
        node.life = 0.f;
        node.maxLife = m_Time;
        node.width = m_TrailWidth;

        // 빌보드???

        _vec3 forward;

        if (!m_Node.empty())
        {
            _vec3 NodePosAPrev = m_Node.back().pos; // 이전
            _vec3 NodePosBCur = curPos; //현제
            _vec3 forward = NodePosBCur - NodePosAPrev;
        }
        else
        {
            forward = { 1,0,0 };// 첫노드용
        }


        D3DXVec3Normalize(&forward, &forward);
        _vec3 camPos = CManagement::GetInstance()->Get_Scene()->Get_MainCamera()->Get_Transform()->Get_Pos();
       // 카메라 값받아와야함 이거 매프레임 마다 바껴야하네
        _vec3 ToCam = camPos - curPos;
        D3DXVec3Normalize(&ToCam, &ToCam);

        _vec3 side;
        D3DXVec3Cross(&side, &forward, &ToCam);
        D3DXVec3Normalize(&side, &side);



        ///

        node.side = side;
        m_Node.push_back(node);
        ///
        m_LastPos = curPos;
    }

    for (auto it = m_Node.begin();it < m_Node.end();)
    {
        it->life += timeDelta;



        if (it->life >= it->maxLife)
        {
            it = m_Node.erase(it);
        }
        else
        {
            it++;
        }

        it->width = m_TrailWidth * (1.0f - it->life / it->maxLife);
    }



    return 0;
}

void CTrailEff::Bill()
{
    //_vec3 forward = curPos - m_LastPos
}

void CTrailEff::LateUpdate_Component()
{
}
void CTrailEff::Render_Effect(CTexture* textureCom, int Num)
{
    if (m_Node.size() < 2)
        return;

    m_GraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
    textureCom->Set_Texture(Num);

    for (int i = 0;i < m_Node.size() - 1; i++)
    {
        TrailNode& A = m_Node[i];
        TrailNode& B = m_Node[i - 1];

        _vec3 A_Left = A.pos - A.side * (A.width * 0.5f);
        _vec3 A_Right = A.pos - A.side * (A.width * 0.5f);

        _vec3 B_Right = B.pos - B.side * (B.width * 0.5f);
        _vec3 B_Left = B.pos - B.side * (B.width * 0.5f);

        TRAILVERTEX v[4];
        v[0] = { A_Left,  0.f, 0.f };
        v[1] = { A_Right, 1.f, 0.f };
        v[2] = { B_Right,  0.f, 1.f };
        v[3] = { B_Left, 1.f, 1.f };

        m_GraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(TRAILVERTEX));
    }

}
CTrailEff* CTrailEff::Create(DEVICE graphicDev)
{
    auto TrailEff = new CTrailEff(graphicDev);

    if (FAILED(TrailEff->Ready_Effect()))
    {
        MSG_BOX("TrailEff Create Failed");
        Safe_Release(TrailEff);
        return nullptr;
    }

    return TrailEff;

}

CComponent* CTrailEff::Clone()
{
    return new CTrailEff(*this);
}

void CTrailEff::Free()
{
    CComponent::Free();
}
