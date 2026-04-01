#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------

CMainFrame::CMainFrame() noexcept
{
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(AfxGetInstanceHandle(), fileName, MAX_PATH);
	::PathRenameExtensionW(fileName, L".xml");
	MY_TRACE_WSTR(fileName);

	m_fileUpdateChecker.init(fileName);
	m_isSettingsFileLoaded = FALSE;

	m_imageVersion = 1;
	m_clamp = FALSE;
	m_horz = _T("left");
	m_vert = _T("center");
	m_alpha = 255;
	m_scale = 100;
	m_selectedColor = Color(0x80, 0xff, 0x00, 0x00);
	m_hotColor = Color(0x80, 0x00, 0xff, 0x00);

	m_currentPart = -1;
	m_hotPart = -1;
	m_targetWnd = NULL;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::loadImage()
{
	MY_TRACE(_T("CMainFrame::loadImage()\n"));

	if (!m_image.IsNull()) m_image.Destroy();
	m_parts.clear();

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(AfxGetInstanceHandle(), path, _countof(path));
	::PathRemoveFileSpec(path);

	switch (m_imageVersion)
	{
	case 3:
		{
			::PathAppend(path, _T("easing3.png"));
			MY_TRACE_TSTR(path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			// (1) \u306e\u4f4d\u706e\u3002
			int x = 20;
			int y = 100;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 160;

				// (2) \u3068 (6) \u306e\u4f4d\u706e\u3002
				int x1 = 20;
				int x2 = 735;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 175;
					x2 += 175;
				}
			}

			break;
		}
	case 2:
		{
			::PathAppend(path, _T("easing2.png"));
			MY_TRACE_TSTR(path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			int x = 20;
			int y = 98;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 155;

				int x1 = 20;
				int x2 = 695;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 163;
					x2 += 163;
				}
			}

			break;
		}
	default:
		{
			::PathAppend(path, _T("easing.png"));
			MY_TRACE_TSTR(path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			int x = 103;
			int y = 40;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 178;

				int x1 = 103;
				int x2 = 649;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 125;
					x2 += 125;
				}
			}

			break;
		}
	}
}

void CMainFrame::setRect(int number, int _x, int _y)
{
//	MY_TRACE(_T("CMainFrame::setRect(%d, %d, %d)\n"), number, _x, _y);

	switch (m_imageVersion)
	{
	case 3:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 160, m_scale, 100);
			int h = ::MulDiv(_y + 120, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	case 2:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 140, m_scale, 100);
			int h = ::MulDiv(_y + 108, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	default:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 121, m_scale, 100);
			int h = ::MulDiv(_y + 101, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	}
}

void CMainFrame::setHotPart(int index)
{
	if (m_hotPart == index)
		return;

	m_hotPart = index;
	Invalidate(FALSE);
}

HWND CMainFrame::getTarget()
{
	HWND hwnd = ::GetForegroundWindow();
	DWORD pid = 0;
	::GetWindowThreadProcessId(hwnd, &pid);

	auto matches = [&](HWND h) {
		TCHAR className[256];
		::GetClassName(h, className, _countof(className));
		TCHAR text[MAX_PATH];
		::GetWindowText(h, text, _countof(text));

		// \u30c0\u30a4\u30a2\u30ed\u30b0\u30af\u30e9\u30b9 (#32770) \u3092\u30c1\u30a7\u30c3\u30af
		if (::lstrcmpi(className, _T("#32770")) != 0)
			return false;

		// AviUtl2 \u3067\u306f\u82f1\u5b57\u306e "easing" \u3084 "@" \u304c\u542b\u307e\u308c\u308b\u3053\u3068\u304c\u591a\u3044
		if (::lstrcmp(text, _T("\u79fb\u52d5\u30d5\u30ec\u30fc\u30e0\u9593\u9694")) == 0 || 
			::StrStrI(text, _T("\u30a4\u30fc\u30b8\u30f3\u30b0")) != NULL ||
			::StrStrI(text, _T("easing")) != NULL ||
			::StrStrI(text, _T("@")) != NULL)
		{
			// Edit \u30af\u30e9\u30b9\u306e\u5b50\u30a6\u30a3\u30f3\u30c9\u30a6\u3092\u6301\u3064\u304b\u3082\u30c1\u30a7\u30c3\u30af
			if (::FindWindowEx(h, NULL, _T("Edit"), NULL) != NULL)
				return true;
		}
		return false;
	};

	if (pid == theApp.m_mainProcessId && matches(hwnd))
		return hwnd;

	// \u30d5\u30a9\u30fc\u30eb\u30d0\u30c3\u30af: \u30bf\u30fc\u30b2\u30c3\u30c8\u30d6\u30ed\u30bb\u30b9\u306e\u89ef\u3048\u308b\u30a6\u30a3\u30f3\u30c9\u30a6\u3092\u63a2\u3059
	struct EnumParam { DWORD pid; HWND hwnd; decltype(matches)* pMatches; };
	EnumParam param = { theApp.m_mainProcessId, NULL, &matches };
	::EnumWindows([](HWND h, LPARAM lp) -> BOOL {
		EnumParam* p = (EnumParam*)lp;
		DWORD pid = 0;
		::GetWindowThreadProcessId(h, &pid);
		if (pid == p->pid && ::IsWindowVisible(h)) {
			if ((*p->pMatches)(h)) {
				p->hwnd = h;
				return FALSE;
			}
		}
		return TRUE;
	}, (LPARAM)&param);

	return param.hwnd;
}

int CMainFrame::getEasing()
{
	if (!::IsWindow(m_targetWnd)) return -1;
	HWND target = m_targetWnd;

	// \u6700\u521d\u306e\u5b50\u30a6\u30a3\u30f3\u30c9\u30a6\u3067\u306f\u306a\u304f\u3001Edit \u30af\u30e9\u30b9\u3092\u660e\u793a\u7684\u306b\u63a2\u3059
	HWND child = ::FindWindowEx(target, NULL, _T("Edit"), NULL);
	if (!child) return -1;

	TCHAR text[MAX_PATH] = {};
	::SendMessage(child, WM_GETTEXT, _countof(text), (LPARAM)text);

	int easing = _ttoi(text);
	if (m_easeWindow.m_enable)
		easing = -easing - 1;
	else
		easing = easing - 1;

	return easing;
}

void CMainFrame::setEasing(int index)
{
	if (!::IsWindow(m_targetWnd)) return;
	HWND target = m_targetWnd;

	int easing = index;
	if (m_easeWindow.m_enable)
		easing = -easing - 1;
	else
		easing = easing + 1;

	// \u6700\u521d\u306e\u5b50\u30a6\u30a3\u30f3\u30c9\u30a6\u3067\u306f\u306a\u304f\u3001Edit \u30af\u30e9\u30b9\u3092\u660e\u793a\u7684\u306b\u63a2\u3059
	HWND child = ::FindWindowEx(target, NULL, _T("Edit"), NULL);
	if (!child) return;

	TCHAR text[MAX_PATH] = {};
	_itot_s(easing, text, 10);
	::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text);
	::PostMessage(target, WM_COMMAND, IDOK, 0);
}

void CMainFrame::show(HWND target)
{
	if (IsWindowVisible())
		return;

	m_currentPart = getEasing();
	m_hotPart = -1;

	CRect rc; GetWindowRect(&rc);
	CRect rcTarget; ::GetWindowRect(target, &rcTarget);

	int x, y;

	if (::lstrcmpi(m_horz, _T("left")) == 0)
	{
		x = rcTarget.left - rc.Width();
	}
	else if (::lstrcmpi(m_horz, _T("right")) == 0)
	{
		x = rcTarget.right;
	}
	else
	{
		x = rcTarget.CenterPoint().x - rc.Width() / 2;
	}

	if (::lstrcmpi(m_vert, _T("top")) == 0)
	{
		y = rcTarget.top - rc.Height();
	}
	else if (::lstrcmpi(m_vert, _T("bottom")) == 0)
	{
		y = rcTarget.bottom;
	}
	else
	{
		y = rcTarget.CenterPoint().y - rc.Height() / 2;
	}

	if (m_clamp)
	{
		HMONITOR monitor = ::MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) }; ::GetMonitorInfo(monitor, &mi);

		if (x < mi.rcWork.left)
			x = mi.rcWork.left;
		else if (x + rc.Width() > mi.rcWork.right)
			x = mi.rcWork.right - rc.Width();

		if (y < mi.rcWork.top)
			y = mi.rcWork.top;
		else if (y + rc.Height() > mi.rcWork.bottom)
			y = mi.rcWork.bottom - rc.Height();
	}

	SetLayeredWindowAttributes(0, m_alpha, LWA_ALPHA);
	SetWindowPos(&wndTopMost, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	Invalidate(FALSE);
}

void CMainFrame::hide()
{
	if (!IsWindowVisible())
		return;

	ShowWindow(SW_HIDE);
}

//--------------------------------------------------------------------

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style = WS_POPUP | FWS_ADDTOTITLE;
	cs.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	MY_TRACE(_T("CMainFrame::OnCreate()\n"));

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!::IsWindow(theApp.m_mainProcessWindow))
	{
		AfxMessageBox(_T("\u30e1\u30a4\u30f3\u30d7\u30ed\u30bb\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u304c\u898b\u3064\u304b\u308a\u307e\u305b\u3093\u3002"));
		return FALSE;
	}

	if (!m_easeWindow.Create(this))
	{
		AfxMessageBox(_T("EaseWindow の作成に失敗しました\n"));
		return -1;
	}

	if (loadSettings() != S_OK)
	{
		// 失敗しても終了せず、デフォルト設定（XMLなし）で画像の読み込みを試みる
		loadImage();
	}

	SetTimer(TIMER_ID, 100, 0);

	return 0;
}

void CMainFrame::OnDestroy()
{
	MY_TRACE(_T("CMainFrame::OnDestroy()\n"));

	KillTimer(TIMER_ID);

	m_image.Destroy();

	saveSettings();

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnPaint()
{
	CPaintDC paintDC(this);

	CUxDC dc(paintDC);
	if (!dc.isValid()) return;

	CRect rc; GetClientRect(&rc);

	if (!m_image.IsNull())
	{
		int w = m_image.GetWidth();
		int h = m_image.GetHeight();
		m_image.AlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0, w, h);
	}

	Graphics g(dc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
//	g.SetCompositingMode(CompositingModeSourceOver);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.TranslateTransform(-0.5f, -0.5f);

	if (m_currentPart >= 0 && m_currentPart < (int)m_parts.size())
	{
		SolidBrush brush(m_selectedColor);
		REAL x = (REAL)m_parts[m_currentPart].left;
		REAL y = (REAL)m_parts[m_currentPart].top;
		REAL w = (REAL)m_parts[m_currentPart].Width();
		REAL h = (REAL)m_parts[m_currentPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}

	if (m_hotPart >= 0 && m_hotPart < (int)m_parts.size())
	{
		SolidBrush brush(m_hotColor);
		REAL x = (REAL)m_parts[m_hotPart].left;
		REAL y = (REAL)m_parts[m_hotPart].top;
		REAL w = (REAL)m_parts[m_hotPart].Width();
		REAL h = (REAL)m_parts[m_hotPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}
}

void CMainFrame::OnTimer(UINT_PTR timerId)
{
	if (timerId == TIMER_ID)
	{
#ifdef SELECT_EASING_CHECK_MAIN_PROCESS
		if (!::IsWindow(theApp.m_mainProcessWindow))
		{
			KillTimer(TIMER_ID);
			PostMessage(WM_CLOSE);
		}
#endif
		HWND target = getTarget();
		m_targetWnd = target;

		if (target)
		{
			show(target);
			m_easeWindow.show(target, GetSafeHwnd());
		}
		else
		{
			m_easeWindow.hide();
			hide();
		}

		if (m_fileUpdateChecker.isFileUpdated())
			loadSettings();
	}

	CFrameWnd::OnTimer(timerId);
}

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	return CFrameWnd::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
//	CFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);

	int size = static_cast<int>(m_parts.size());
	for (int i = 0; i < size; i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setHotPart(i);
			return HTCLIENT;
		}
	}

	setHotPart(-1);
	return HTCAPTION;
//	return CFrameWnd::OnNcHitTest(point);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	int size = static_cast<int>(m_parts.size());
	for (int i = 0; i < size; i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setEasing(i);

			return;
		}
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}

//--------------------------------------------------------------------
