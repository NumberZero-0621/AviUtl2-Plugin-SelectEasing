#pragma once

#include <Windows.h>
#include <aviutl2_sdk/plugin2.h>
#include <aviutl2_sdk/filter2.h>

class CInProcessApp
{
public:

	HINSTANCE m_instance;
	HWND m_hwnd;
	struct HOST_APP_TABLE* m_host;
	PROCESS_INFORMATION m_pi;
	int m_trackIndex;

	CInProcessApp();
	virtual ~CInProcessApp();

	BOOL dllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	void registerPlugin(struct HOST_APP_TABLE* host);
	bool initializePlugin(DWORD version);
	void uninitializePlugin();

	BOOL createSubProcess();
	void initHook();
	void termHook();
};

extern CInProcessApp theApp;
