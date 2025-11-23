#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CDInputMgr)

CDInputMgr::CDInputMgr()
    : m_InputSDK(nullptr), m_KeyBoardDev(nullptr), m_MouseDev(nullptr),
      m_KeyState{}, m_MouseState{}
{ }

CDInputMgr::~CDInputMgr()
{
    CDInputMgr::Free();
}

HRESULT CDInputMgr::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
    if (FAILED(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_InputSDK,
        nullptr)))
        return E_FAIL;

    if (FAILED(m_InputSDK->CreateDevice(GUID_SysKeyboard, &m_KeyBoardDev, nullptr)))
        return E_FAIL;

    m_KeyBoardDev->SetDataFormat(&c_dfDIKeyboard);

    m_KeyBoardDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    m_KeyBoardDev->Acquire();

    if (FAILED(m_InputSDK->CreateDevice(GUID_SysMouse, &m_MouseDev, nullptr)))
        return E_FAIL;

    m_MouseDev->SetDataFormat(&c_dfDIMouse);

    m_MouseDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    m_MouseDev->Acquire();

    m_hWnd = hWnd;

    return S_OK;
}

void CDInputMgr::Update_InputDev()
{
    m_KeyBoardDev->GetDeviceState(256, m_KeyState);
    m_MouseDev->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void CDInputMgr::Free()
{
    Safe_Release(m_KeyBoardDev);
    Safe_Release(m_MouseDev);
    Safe_Release(m_InputSDK);
}
