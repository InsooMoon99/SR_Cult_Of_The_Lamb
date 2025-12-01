#include "pch.h"
#include "CLoading.h"
#include "CAnimator.h"
#include "CBoxCol.h"
#include "CProtoMgr.h"
#include "CResourceMgr.h"

CLoading::CLoading(DEVICE pGraphicDev)
    : m_GraphicDev(pGraphicDev),
    m_Thread(nullptr),
    m_Crt{},
    m_LoadingID(LOADING_END),
    m_IsFinish(false)
{
    m_GraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
    // 스레드 동기화를 위한 임계영역(크리티컬 섹션) 초기화
    InitializeCriticalSection(&m_Crt);

    // 로딩 스레드 생성
    m_Thread = (HANDLE)_beginthreadex(
        nullptr,	        // 보안속성(핸들의 상속 여부, NULL인 경우 상속에서 제외)
        0,		            // 스택 크기 (0 = OS 기본값 사용)
        Thread_Main,		// 쓰레드 함수(__stdcall 호출 규약)
        this,	            // 쓰레드 함수를 이용하여 가공할 데이터 주소
        0,		            // 쓰레드 생성 및 실행을 조절하는 flag
        nullptr);	        // 쓰레드 id 반환

    m_LoadingID = eID;

    return S_OK;
}

_uint CLoading::Loading_ForState()
{
    m_LoadingText = L"Resource Loading...";

    // 이제 리소스 추가는 Resource Manager에서 진행
    if (FAILED(CResourceMgr::GetInstance()->Ready_Resource(m_GraphicDev)))
        return E_FAIL;

    m_IsFinish = true;

    return S_OK;
}

_uint CLoading::Loading_ForBoss()
{
    m_LoadingText = L"Boss Loading...";

    // 테스트 용으로 일단은 플레이어, 몬스터 이미지까지 로딩 진행
    // 나중에는 분리를 해야할 것임
    //if (FAILED(CResourceMgr::GetInstance()->Ready_Resource(m_GraphicDev)))
    //    return E_FAIL;

    if (FAILED(CResourceMgr::GetInstance()->Ready_BossResource(m_GraphicDev)))
        return E_FAIL;

    m_IsFinish = true;

    return S_OK;
}

unsigned CLoading::Thread_Main(void* pArg)
{
    auto pLoading = static_cast<CLoading*>(pArg);

    _uint iFlag(0);

    // 임계영역 진입 (다른 스레드의 접근 차단)
    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_ForState();
        break;
    case LOADING_BOSS:
        iFlag = pLoading->Loading_ForBoss();
        break;
    default:
        break;
    }

    // 임계영역 해제 (다른 스레드 접근 허용)
    LeaveCriticalSection(pLoading->Get_Crt());

    //_endthreadex(0);
    // -> _beginthreadex()로 생성된 스레드는 함수가 return 될 때
    // 내부적으로 _endthreadex() 가 자동 호출되어 정리됨.
    // 따라서 명시적으로 호출할 필요가 없음

    return iFlag;
}

CLoading* CLoading::Create(DEVICE pGraphicDev, LOADINGID eID)
{
    auto pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);

        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    // 생성한 스레드가 완전히 끝날 때까지 대기
    WaitForSingleObject(m_Thread, INFINITE);

    // 스레드 핸들 닫기 (핸들 리소스 해제)
    CloseHandle(m_Thread);

    // 크리티컬 섹션 삭제 (운영체제 리소스 해제)
    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_GraphicDev);
}
