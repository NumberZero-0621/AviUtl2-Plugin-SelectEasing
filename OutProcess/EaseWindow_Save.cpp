#include "pch.h"
#include "OutProcess.h"
#include "EaseWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------

// <ease> \u3092\u4f5c\u6210\u3059\u308b\u3002
HRESULT CEaseWindow::saveEase(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("CEaseWindow::saveEase()\n"));

	// <ease> \u3092\u4f5c\u6210\u3059\u308b\u3002
	MSXML2::IXMLDOMElementPtr easeElement = appendElement(element, L"ease");

	// <ease> \u306e\u30a2\u30c8\u30ea\u30d3\u30e5\u30fc\u30c8\u3092\u4f5c\u6210\u3059\u308b\u3002
	setPrivateProfileBool(easeElement, L"enable", m_enable);
	setPrivateProfileString(easeElement, L"origin", m_origin);
	setPrivateProfileBool(easeElement, L"clamp", m_clamp);
	setPrivateProfileString(easeElement, L"horz", m_horz);
	setPrivateProfileString(easeElement, L"vert", m_vert);
	setPrivateProfileInt(easeElement, L"alpha", m_alpha);
	setPrivateProfileInt(easeElement, L"margin", m_margin);
	setPrivateProfileInt(easeElement, L"hitDistance", m_hitDistance);
	setPrivateProfileInt(easeElement, L"segmentCount", m_segmentCount);
	setPrivateProfileBool(easeElement, L"hideCursor", m_hideCursor);
	setPrivateProfileBool(easeElement, L"immediately", m_immediately);

	{
		MSXML2::IXMLDOMElementPtr windowElement = appendElement(easeElement, L"window");

		setPrivateProfileInt(windowElement, L"w", m_windowSize.cx);
		setPrivateProfileInt(windowElement, L"h", m_windowSize.cy);
	}

	{
		MSXML2::IXMLDOMElementPtr backgroundElement = appendElement(easeElement, L"background");

		setPrivateProfileColor(backgroundElement, m_backgroundColor);
	}

	{
		MSXML2::IXMLDOMElementPtr borderElement = appendElement(easeElement, L"border");

		setPrivateProfileColor(borderElement, m_borderColor);
		setPrivateProfileInt(borderElement, L"width", m_borderWidth);
	}

	{
		MSXML2::IXMLDOMElementPtr curveElement = appendElement(easeElement, L"curve");

		setPrivateProfileColor(curveElement, m_curveColor);
		setPrivateProfileInt(curveElement, L"width", m_curveWidth);
	}

	{
		MSXML2::IXMLDOMElementPtr invalidCurveElement = appendElement(easeElement, L"invalidCurve");

		setPrivateProfileColor(invalidCurveElement, m_invalidCurveColor);
		setPrivateProfileInt(invalidCurveElement, L"width", m_invalidCurveWidth);
	}

	{
		MSXML2::IXMLDOMElementPtr handleElement = appendElement(easeElement, L"handle");

		setPrivateProfileColor(handleElement, m_handleColor);
		setPrivateProfileInt(handleElement, L"width", m_handleWidth);
	}

	{
		MSXML2::IXMLDOMElementPtr pointElement = appendElement(easeElement, L"point");

		setPrivateProfileColor(pointElement, m_pointColor);
		setPrivateProfileInt(pointElement, L"radius", m_pointRadius);
	}

	{
		MSXML2::IXMLDOMElementPtr hotPointElement = appendElement(easeElement, L"hotPoint");

		setPrivateProfileColor(hotPointElement, m_hotPointColor);
	}

	return S_OK;
}

//--------------------------------------------------------------------
