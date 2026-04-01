#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"
#include "../../Common-Library/Common/Tracer.h"
#include "../../Common-Library/Common/Tracer2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// \u30d7\u30ed\u30bb\u30b9ID\u304b\u3089\u30e1\u30a4\u30f3\u30a6\u30a3\u30f3\u30c9\u30a6\u3092\u7279\u5b9a\u3059\u308b\u305f\u3081\u306e\u30b3\u30fc\u30eb\u30d0\u30c3\u30af\u7528\u69cb\u9020\u4f53
struct EnumData {
	DWORD pid;
	HWND hwnd;
};

// \u30e1\u30a4\u30f3\u30a6\u30a3\u30f3\u30c9\u30a6\u3092\u63a2\u3059\u30b3\u30fc\u30eb\u30d0\u30c3\u30af
BOOL CALLBACK EnumMainWindowProc(HWND hwnd, LPARAM lParam) {
	EnumData* pData = (EnumData*)lParam;
	DWORD pid = 0;
	::GetWindowThreadProcessId(hwnd, &pid);
	if (pid == pData->pid) {
		TCHAR className[256];
		::GetClassName(hwnd, className, _countof(className));
		// AviUtl \u3084 AviUtl2 \u306e\u30e1\u30a4\u30f3\u30a6\u30a3\u30f3\u30c9\u30a6\u30af\u30e9\u30b9\u540d\u3092\u30c1\u30a7\u30c3\u30af
		if (::lstrcmpi(className, _T("aviutl2Manager")) == 0 || ::lstrcmpi(className, _T("AviUtl")) == 0) {
			pData->hwnd = hwnd;
			return FALSE;
		}
	}
	return TRUE;
}

COutProcessApp theApp;

BEGIN_MESSAGE_MAP(COutProcessApp, CWinApp)
	ON_THREAD_MESSAGE(WM_SELECT_EASING_NOTIFY, OnSelectEasingNotify)
END_MESSAGE_MAP()

COutProcessApp::COutProcessApp() noexcept
{
	trace_init(0, 0);

	m_mainProcessWindow = 0;
	m_mainProcessId = 0;
}

COutProcessApp::~COutProcessApp()
{
	trace_term();
}

BOOL COutProcessApp::InitInstance()
{
	{
		int argc = 0;
		LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
		if (argv && argc > 1)
		{
			// \u7b2c\u0031\u5f15\u6570 (argv[1]) \u306b HWND \u306e 16\u9032\u6587\u5b57\u5217\u304c\u6e21\u3055\u308c\u3066\u3044\u308b
			m_mainProcessWindow = (HWND)_wcstoui64(argv[1], 0, 16);
			
			if (argc > 2)
			{
				// \u7b2c\u0032\u5f15\u6570 (argv[2]) \u306b PID \u304c\u6e21\u3055\u308c\u3066\u3044\u308b
				m_mainProcessId = _wtoi(argv[2]);
			}
			else
			{
				::GetWindowThreadProcessId(m_mainProcessWindow, &m_mainProcessId);
			}
			
			::LocalFree(argv);
		}
		else if (argv)
		{
			::LocalFree(argv);
		}

		MY_TRACE_HWND(m_mainProcessWindow);

#ifdef SELECT_EASING_CHECK_MAIN_PROCESS
		// HWND がNULLでも PID があれば探す
		if ((!m_mainProcessWindow || !::IsWindow(m_mainProcessWindow)) && m_mainProcessId != 0)
		{
			EnumData data = { m_mainProcessId, NULL };
			::EnumWindows(EnumMainWindowProc, (LPARAM)&data);
			m_mainProcessWindow = data.hwnd;
			MY_TRACE(_T("Search for MainWindow by PID(%u) = 0x%08X\n"), m_mainProcessId, m_mainProcessWindow);
		}

		if (!::IsWindow(m_mainProcessWindow))
		{
			AfxMessageBox(_T("\u30e1\u30a4\u30f3\u30d7\u30ed\u30bb\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u304c\u898b\u3064\u304b\u308a\u307e\u305b\u3093\u3002"));
			return FALSE;
		}
#endif
	}

	CWinApp::InitInstance();

	if (!AfxOleInit())
		return -1;

	EnableTaskbarInteraction(FALSE);

	CFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, 0, nullptr, nullptr);

	return TRUE;
}

int COutProcessApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

int COutProcessApp::Run()
{
	GdiplusStartupInput gdiSI;
	GdiplusStartupOutput gdiSO;
	ULONG_PTR gdiToken = 0;
	ULONG_PTR gdiHookToken = 0;

	gdiSI.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiToken, &gdiSI, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	int result = CWinApp::Run();

	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiToken);

	return result;
}

void COutProcessApp::OnSelectEasingNotify(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("COutProcessApp::OnSelectEasingNotify(0x%08X, 0x%08X)\n"), wParam, lParam);
#if 1
	m_pMainWnd->PostMessage(WM_TIMER, TIMER_ID, 0);
#else
	CMainFrame* mainFrame = (CMainFrame*)m_pMainWnd;
	mainFrame->OnTimer(TIMER_ID);
#endif
}
