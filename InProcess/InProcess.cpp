#include <cstdint>
#include <Windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <stdio.h>

#pragma comment(lib, "shlwapi.lib")

#include "../../aviutl2_sdk_mirror/include/aviutl2_sdk/plugin2.h"
#include "../../aviutl2_sdk_mirror/include/aviutl2_sdk/filter2.h"
#include "InProcess.h"
#include "../../Common-Library/Common/Tracer.h"

//--------------------------------------------------------------------

CInProcessApp theApp;

// デバッグ出力用の関数は Tracer2.h で inline 定義されるため、ここでは定義しない。

CInProcessApp::CInProcessApp()
{
	m_instance = 0;
	m_hwnd = 0;
	m_host = nullptr;
	m_trackIndex = 0;
	::ZeroMemory(&m_pi, sizeof(m_pi));
}

CInProcessApp::~CInProcessApp()
{
}

BOOL CInProcessApp::dllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			m_instance = instance;
			break;
		}
	}

	return TRUE;
}

void CInProcessApp::registerPlugin(struct HOST_APP_TABLE* host)
{
	m_host = host;
}

#include "../../Common-Library/Common/Tracer2.h"

bool CInProcessApp::initializePlugin(DWORD version)
{
	trace_init(m_instance, _T("_InProcess"));
	MY_TRACE(_T("CInProcessApp::initializePlugin(%u)\n"), version);

	if (m_host && m_host->create_edit_handle)
	{
		auto edit = m_host->create_edit_handle();
		if (edit && edit->get_host_app_window)
			m_hwnd = edit->get_host_app_window();
	}

	// AviUtl2 \u306e\u30e1\u30a4\u30f3\u30a6\u30a3\u30f3\u30c9\u30a6\u3092\u30af\u30e9\u30b9\u540d\u3067\u63a2\u3059 (\u30d5\u30a9\u30fc\u30eb\u30d0\u30c3\u30af)
	if (!m_hwnd || !::IsWindow(m_hwnd))
	{
		m_hwnd = ::FindWindow(_T("aviutl2Manager"), NULL);
		MY_TRACE(_T("Fallback: FindWindow(aviutl2Manager) = 0x%08X\n"), m_hwnd);
	}

	createSubProcess();
	return true;
}

void CInProcessApp::uninitializePlugin()
{
	MY_TRACE(_T("CInProcessApp::uninitializePlugin()\n"));
	if (m_pi.hProcess)
	{
		::CloseHandle(m_pi.hThread);
		::CloseHandle(m_pi.hProcess);
		::ZeroMemory(&m_pi, sizeof(m_pi));
	}
	trace_term();
}

BOOL CInProcessApp::createSubProcess()
{
	MY_TRACE(_T("CInProcessApp::createSubProcess()\n"));
	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(m_instance, path, MAX_PATH);
	::PathRemoveFileSpec(path);
	::PathAppend(path, _T("SelectEasing"));
	::PathAppend(path, _T("SelectEasing.exe"));

	TCHAR args[MAX_PATH] = {};
	// HWND \u306f NULL \u306e\u5834\u5408\u308d\u304c\u308b\u3068\u3044\u3051\u306a\u3044\u306e\u3067\u30010 \u3068\u3057\u3066\u6e21\u3059
	::StringCbPrintf(args, sizeof(args), _T("\"%s\" 0x%016p %u"), path, m_hwnd, ::GetCurrentProcessId());
	MY_TRACE_TSTR(args);

	STARTUPINFO si = { sizeof(si) };
	if (!::CreateProcess(path, args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &m_pi))
	{
		TCHAR error[MAX_PATH];
		::StringCbPrintf(error, sizeof(error), _T("SelectEasing.exe の起動に失敗しました。(Error: %u)"), ::GetLastError());
		::MessageBox(m_hwnd, error, _T("SelectEasing エラー"), MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

void CInProcessApp::initHook() {}
void CInProcessApp::termHook() {}

//--------------------------------------------------------------------
// \u30a8\u30af\u30b9\u30dd\u30fc\u30c8\u95a2\u6570\u306e\u5b9f\u88c5\uff08SDK \u306e\u5b9a\u7fa9\u306b\u53b3\u5bc6\u306b\u5408\u308f\u305b\u3001\u540d\u524d\u4fee\u98fe\u3068\u547c\u3073\u51fa\u3057\u898f\u7d04\u306e\u4e0d\u4e00\u81f4\u3092\u89e3\u6d88\uff09
//--------------------------------------------------------------------

extern "C" {

	__declspec(dllexport) void RegisterPlugin(struct HOST_APP_TABLE* host)
	{
		theApp.registerPlugin(host);
	}

	__declspec(dllexport) bool InitializePlugin(DWORD version)
	{
		return theApp.initializePlugin(version);
	}

	__declspec(dllexport) void UninitializePlugin()
	{
		theApp.uninitializePlugin();
	}

	__declspec(dllexport) struct COMMON_PLUGIN_TABLE* GetCommonPluginTable()
	{
		static struct COMMON_PLUGIN_TABLE common = {};
		common.name = L"\u30a4\u30fc\u30b8\u30f3\u30b0\u7c21\u53d8\u9078\u629e";
		common.information = L"\u30a4\u30fc\u30b8\u30f3\u30b0\u7c21\u53d8\u9078\u629e 4.5.0 by \u86c7\u8272 (AviUtl2\u5bfe\u5fdc)";

		return &common;
	}

} // extern "C"

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.dllMain(instance, reason, reserved);
}
