#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------

HRESULT CMainFrame::loadSettings()
{
	MY_TRACE(_T("CMainFrame::loadSettings()\n"));

	try
	{
		m_isSettingsFileLoaded = FALSE;

		LPCWSTR fileName = m_fileUpdateChecker.getFilePath();

		// MSXML \u3092\u4f7f\u7528\u3059\u308b\u3002
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// \u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u3092\u958b\u304f\u3002
		if (document->load(fileName) == VARIANT_FALSE)
		{
			MY_TRACE(_T("%s \u3092\u958b\u3051\u307e\u305b\u3093\u3067\u3057\u305f\n"), fileName);

			return S_FALSE;
		}

		MSXML2::IXMLDOMElementPtr element = document->documentElement;

		// <easing> \u3092\u8aad\u307f\u8fbc\u3080\u3002
		loadEasing(element);

		// <ease> \u3092\u8aad\u307f\u8fbc\u3080\u3002
		m_easeWindow.loadEase(element);

		MY_TRACE(_T("\u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u306e\u8aad\u307f\u8fbc\u307f\u306b\u6210\u529f\u3057\u307e\u3057\u305f\n"));

		// \u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u3092\u8aad\u307f\u8fbc\u3081\u305f\u306e\u3067\u30d5\u30e9\u30b0\u3092\u7acb\u3066\u3066\u304a\u304f\u3002
		m_isSettingsFileLoaded = TRUE;

		loadImage();

		return S_OK;
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("\u8a2d\u5b9a\u30d5\u30a1\u30a4\u30eb\u306e\u8aad\u307f\u8fbc\u307f\u306b\u5931\u6557\u3057\u307e\u3057\u305f\n"));
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <easing> \u3092\u8aad\u307f\u8fbc\u3080\u3002
HRESULT CMainFrame::loadEasing(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("CMainFrame::loadEasing()\n"));

	// <easing> \u3092\u53d6\u5f97\u3059\u308b\u3002
	MSXML2::IXMLDOMNodeListPtr nodeList = element->getElementsByTagName(L"easing");
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr easingElement = nodeList->item[i];

		// <easing> \u306e\u30a2\u30c8\u30ea\u30d3\u30e5\u30fc\u30c8\u3092\u53d6\u5f97\u3059\u308b\u3002
		getPrivateProfileInt(easingElement, L"imageVersion", m_imageVersion);
		getPrivateProfileBool(easingElement, L"clamp", m_clamp);
		getPrivateProfileString(easingElement, L"horz", m_horz);
		getPrivateProfileString(easingElement, L"vert", m_vert);
		getPrivateProfileInt(easingElement, L"alpha", m_alpha);
		getPrivateProfileInt(easingElement, L"scale", m_scale);

		{
			// <selected> \u3092\u53d6\u5f97\u3059\u308b\u3002
			MSXML2::IXMLDOMNodeListPtr nodeList = easingElement->getElementsByTagName(L"selected");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr selectedElement = nodeList->item[i];

				getPrivateProfileColor(selectedElement, m_selectedColor);
			}
		}

		{
			// <hot> \u3092\u53d6\u5f97\u3059\u308b\u3002
			MSXML2::IXMLDOMNodeListPtr nodeList = easingElement->getElementsByTagName(L"hot");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr hotElement = nodeList->item[i];

				getPrivateProfileColor(hotElement, m_hotColor);
			}
		}
	}

	return S_OK;
}

//--------------------------------------------------------------------
