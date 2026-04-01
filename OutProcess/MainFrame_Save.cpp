#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CMainFrame::saveSettings()
{
	MY_TRACE(_T("CMainFrame::saveSettings()\n"));

	// \u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u3092\u8aad\u307f\u8fbc\u3081\u3066\u3044\u306a\u3044\u5834\u5408\u306f\u3001\u4fdd\u5b58\u51e6\u7406\u3092\u3059\u308b\u3068\u30d5\u30a1\u30a4\u30eb\u306e\u4e2d\u306e\u8a2d\u5b9a\u5024\u304c\u3059\u3079\u3066\u521d\u671f\u5024\u306b\u6238\u3063\u3066\u3057\u307e\u3046\u3002
	// \u3088\u3063\u3066\u3001\u30d5\u30e9\u30b0\u3092\u78ba\u8a8d\u3057\u3066\u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u3092\u8aad\u307f\u8fbc\u3081\u3066\u3044\u306a\u3044\u5834\u5408\u306f\u4fdd\u5b58\u51e6\u7406\u3092\u3057\u306a\u3044\u3088\u3046\u306b\u3059\u308b\u3002
	if (!m_isSettingsFileLoaded)
		return S_FALSE;

	try
	{
		// \u30c9\u30ad\u30e5\u30e1\u30f3\u30c8\u3092\u4f5c\u6210\u3059\u308b\u3002
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// \u30c9\u30ad\u30e5\u30e1\u30f3\u30c8\u30a8\u30ec\u30e1\u30f3\u30c8\u3092\u4f5c\u6210\u3059\u308b\u3002
		MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"settings");

		// <easing> \u3092\u4f5c\u6210\u3059\u308b\u3002
		saveEasing(element);

		// <ease> \u3092\u4f5c\u6210\u3059\u308b\u3002
		m_easeWindow.saveEase(element);

		return saveXMLDocument(document, m_fileUpdateChecker.getFilePath(), L"UTF-16");
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <easing> \u3092\u4f5c\u6210\u3059\u308b\u3002
HRESULT CMainFrame::saveEasing(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("CMainFrame::saveEasing()\n"));

	// <easing> \u3092\u4f5c\u6210\u3059\u308b\u3002
	MSXML2::IXMLDOMElementPtr easingElement = appendElement(element, L"easing");

	// <easing> \u306e\u30a2\u30c8\u30ea\u30d3\u30e5\u30fc\u30c8\u3092\u4f5c\u6210\u3059\u308b\u3002
	setPrivateProfileInt(easingElement, L"imageVersion", m_imageVersion);
	setPrivateProfileBool(easingElement, L"clamp", m_clamp);
	setPrivateProfileString(easingElement, L"horz", m_horz);
	setPrivateProfileString(easingElement, L"vert", m_vert);
	setPrivateProfileInt(easingElement, L"alpha", m_alpha);
	setPrivateProfileInt(easingElement, L"scale", m_scale);

	{
		MSXML2::IXMLDOMElementPtr selectedElement = appendElement(easingElement, L"selected");

		setPrivateProfileColor(selectedElement, m_selectedColor);
	}

	{
		MSXML2::IXMLDOMElementPtr hotElement = appendElement(easingElement, L"hot");

		setPrivateProfileColor(hotElement, m_hotColor);
	}

	return S_OK;
}
