#include <Windows.h>
#include "../../aviutl2_sdk_mirror/include/aviutl2_sdk/plugin2.h"
#include "../../aviutl2_sdk_mirror/include/aviutl2_sdk/filter2.h"
#include "InProcess.h"
#include "../../Common/Tracer.h"

//--------------------------------------------------------------------

EXTERN_C void CALLBACK RegisterPlugin(HOST_APP_TABLE* host)
{
	// MY_TRACE(_T("RegisterPlugin()\n"));

	theApp.registerPlugin(host);
}

EXTERN_C bool CALLBACK InitializePlugin(DWORD version)
{
	// MY_TRACE(_T("InitializePlugin()\n"));

	return theApp.initializePlugin(version);
}

EXTERN_C void CALLBACK UninitializePlugin()
{
	// MY_TRACE(_T("UninitializePlugin()\n"));

	theApp.uninitializePlugin();
}

EXTERN_C FILTER_PLUGIN_TABLE* CALLBACK GetFilterPluginTable()
{
	static FILTER_PLUGIN_TABLE filter = {};
	
	filter.flag = FILTER_PLUGIN_TABLE::FLAG_VIDEO | FILTER_PLUGIN_TABLE::FLAG_FILTER;
	filter.name = L"\u30a4\u30fc\u30b8\u30f3\u30b0\u7c21\u53d8\u9078\u629e";
	filter.information = L"\u30a4\u30fc\u30b8\u30f3\u30b0\u7c21\u53d8\u9078\u629e 4.5.0 by \u86c7\u8272 (AviUtl2\u5bfe\u5fdc)";

	return &filter;
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.dllMain(instance, reason, reserved);
}

//--------------------------------------------------------------------
