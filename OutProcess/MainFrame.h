#pragma once

#include "EaseWindow.h"

//--------------------------------------------------------------------

class CMainFrame : public CFrameWnd
{
public:

	typedef std::vector<CRect> CParts;

public:

	CEaseWindow m_easeWindow;

	FileUpdateChecker m_fileUpdateChecker;
	BOOL m_isSettingsFileLoaded;

	int m_imageVersion;
	BOOL m_clamp;
	_bstr_t m_horz;
	_bstr_t m_vert;
	int m_alpha;
	int m_scale;
	Color m_selectedColor;
	Color m_hotColor;
	CImage m_image;
	CParts m_parts;
	int m_currentPart;
	int m_hotPart;
	HWND m_targetWnd; // \u691c\u51fa\u3057\u305f\u30bf\u30a4\u30c8\u30eb\u30a2\u30c3\u30d7\uff08\u30c0\u30a4\u30a2\u30ed\u30b3\uff09\u306e\u30cf\u30f3\u30c9\u30eb

public:

	CMainFrame() noexcept;
	virtual ~CMainFrame();

	HRESULT loadSettings();
	HRESULT loadEasing(const MSXML2::IXMLDOMElementPtr& element);
	HRESULT saveSettings();
	HRESULT saveEasing(const MSXML2::IXMLDOMElementPtr& element);
	void loadImage();
	void setRect(int number, int x, int y);
	void setHotPart(int index);
	HWND getTarget();
	int getEasing();
	void setEasing(int index);
	void show(HWND target);
	void hide();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------
