
// AsMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AsMonitor.h"
#include "AsMonitorDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include "FileVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WNDINFO* pInfo = (WNDINFO*)lParam;
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if ((pInfo->dwProcessId != dwProcessId) || (GetParent(hWnd) != NULL))
	{
		return 1;
	}
	pInfo->hWnd = hWnd;
	return 0;
}

HWND CAsMonitorDlg::GetHandleByProcessId(DWORD dwProcessId)
{
	WNDINFO info = { 0 };
	info.hWnd = NULL;
	info.dwProcessId = dwProcessId;
	EnumWindows(EnumWindowsProc, (LPARAM)&info);
	return info.hWnd;
}

LPTSTR CAsMonitorDlg::GetProcessNameById(DWORD dwProcessId)
{
	//获取进程快照
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// 创建快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	LPTSTR lpName = NULL;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (pe.th32ProcessID == dwProcessId)
		{
			lpName = pe.szExeFile;
			break;
		}
		else
		{
			bResult = Process32Next(hSnapshot, &pe);
		}
	}
	//清除hSnapshot对象
	::CloseHandle(hSnapshot);
	// 注意，在Win NT中，要删除快照，使用CloseHandle函数；
	// 在Win CE中，要删除快照，使用CloseToolhelp32Snapshot函数。
	return lpName;
}

DWORD CAsMonitorDlg::GetProcessIdByName(LPTSTR lpszName)
{
	//获取进程快照
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// 创建快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwProcessId = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (_tcscmp(lpszName,pe.szExeFile) == 0)
		{
			dwProcessId = pe.th32ProcessID;
			break;
		}
		else
		{
			bResult = Process32Next(hSnapshot, &pe);
		}
	}
	//清除hSnapshot对象
	::CloseHandle(hSnapshot);
	// 注意，在Win NT中，要删除快照，使用CloseHandle函数；
	// 在Win CE中，要删除快照，使用CloseToolhelp32Snapshot函数。
	return dwProcessId;
}

DWORD CAsMonitorDlg::GetProcessIdByName(LPTSTR lpszName, LPDWORD* lpIds)
{
	//获取进程快照
	if (lpIds == 0)
	{
		return 0;
	}
	DWORD i = 0, Ids = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// 创建快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwProcessId = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (_tcscmp(lpszName,pe.szExeFile) == 0)
		{
			Ids++;
		}
		bResult = Process32Next(hSnapshot, &pe);
	}
	*lpIds = new DWORD[Ids];
	i = 0;
	bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (_tcscmp(lpszName,pe.szExeFile) == 0)
		{
			*(*lpIds+i) = pe.th32ProcessID;
			i++;
		}
		bResult = Process32Next(hSnapshot, &pe);
	}
	//清除hSnapshot对象
	::CloseHandle(hSnapshot);
	// 注意，在Win NT中，要删除快照，使用CloseHandle函数；
	// 在Win CE中，要删除快照，使用CloseToolhelp32Snapshot函数。
	return Ids;
}

DWORD CAsMonitorDlg::GetProcessIdByOrigName(LPTSTR lpszName, LPDWORD* lpIds)
{
	//获取进程快照
	if (lpIds == 0)
	{
		return 0;
	}
	DWORD i = 0, Ids = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// 创建快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwProcessId = 0;
	PROCESSENTRY32 pe;
	wchar_t wszFullPath[MAX_PATH] = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (GetProcessFullPath(pe.th32ProcessID, wszFullPath))
		{
			version::CFileVersionInfo fv(wszFullPath);
			if (_tcscmp(lpszName, fv.GetOriginalTrademarks().c_str()) == 0)
			{
				Ids++;
			}
		}
		bResult = Process32Next(hSnapshot, &pe);
	}
	*lpIds = new DWORD[Ids];
	i = 0;
	bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (GetProcessFullPath(pe.th32ProcessID, wszFullPath))
		{
			version::CFileVersionInfo fv(wszFullPath);
			if (_tcscmp(lpszName, fv.GetOriginalTrademarks().c_str()) == 0)
			{
				*(*lpIds + i) = pe.th32ProcessID;
				i++;
			}
		}
		bResult = Process32Next(hSnapshot, &pe);
	}
	//清除hSnapshot对象
	::CloseHandle(hSnapshot);
	// 注意，在Win NT中，要删除快照，使用CloseHandle函数；
	// 在Win CE中，要删除快照，使用CloseToolhelp32Snapshot函数。
	return Ids;
}

HWND CAsMonitorDlg::GetHandleByTitle(LPTSTR lpProcessName,LPTSTR lpszName)
{
	//获取进程快照
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// 创建快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwProcessId = 0;
	HWND hWnd = 0;
	TCHAR wszTitle[MAX_PATH] = { 0 };
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL bResult = Process32First(hSnapshot, &pe);
	while (bResult)
	{
		if (_tcscmp(lpProcessName, pe.szExeFile) == 0)
		{
			hWnd = GetHandleByProcessId(pe.th32ProcessID);
			::GetWindowText(hWnd, wszTitle, MAX_PATH);
			if (_tcscmp(lpszName, wszTitle) == 0)
			{
				dwProcessId = pe.th32ProcessID;
				break;
			}
			bResult = Process32Next(hSnapshot, &pe);
		}
		else
		{
			bResult = Process32Next(hSnapshot, &pe);
		}
	}
	//清除hSnapshot对象
	::CloseHandle(hSnapshot);
	// 注意，在Win NT中，要删除快照，使用CloseHandle函数；
	// 在Win CE中，要删除快照，使用CloseToolhelp32Snapshot函数。
	return hWnd;
}

// CAsMonitorDlg 对话框



CAsMonitorDlg::CAsMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ASMONITOR_DIALOG, pParent)
	, m_nMjValue(1)
	, m_nTeamValue(4)
	, m_nDrValue(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ImgLst.Create(32, 32, ILC_COLOR32 | ILC_MASK, 3, 1);
	m_ImgLst.Add(AfxGetApp()->LoadIcon(IDR_OFF));
	m_ImgLst.Add(AfxGetApp()->LoadIcon(IDR_ON));
	m_ImgLst.Add(AfxGetApp()->LoadIcon(IDR_PAUSE));
}

void CAsMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MJVALUE, m_nMjValue);
	DDV_MinMaxByte(pDX, m_nMjValue, 1, 15);
	DDX_Text(pDX, IDC_TEAMVALUE, m_nTeamValue);
	DDV_MinMaxUInt(pDX, m_nTeamValue, 1, 15);
	DDX_Text(pDX, IDC_DRVALUE, m_nDrValue);
	DDV_MinMaxUInt(pDX, m_nDrValue, 1, 15);
}

BEGIN_MESSAGE_MAP(CAsMonitorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SHELLNOTIFY,ShellIconNofity)
	ON_BN_CLICKED(IDC_RUN, &CAsMonitorDlg::OnBnClickedRun)
	ON_BN_CLICKED(IDC_HIDE, &CAsMonitorDlg::OnBnClickedHide)
	ON_NOTIFY(NM_CLICK, IDC_H5, &CAsMonitorDlg::OnNMClickH5)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXIT, &CAsMonitorDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_CLOSE, &CAsMonitorDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_STOP, &CAsMonitorDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_GH, &CAsMonitorDlg::OnBnClickedGh)
	ON_BN_CLICKED(IDC_SY, &CAsMonitorDlg::OnBnClickedSy)
	ON_BN_CLICKED(IDC_MJ, &CAsMonitorDlg::OnBnClickedMj)
	ON_BN_CLICKED(IDC_ZBTX, &CAsMonitorDlg::OnBnClickedZbtx)
	ON_BN_CLICKED(IDC_MLD, &CAsMonitorDlg::OnBnClickedMld)
	ON_BN_CLICKED(IDC_MLX, &CAsMonitorDlg::OnBnClickedMlx)
	ON_BN_CLICKED(IDC_BYFL, &CAsMonitorDlg::OnBnClickedByfl)
	ON_BN_CLICKED(IDC_TTSD, &CAsMonitorDlg::OnBnClickedTtsd)
	ON_BN_CLICKED(IDC_YSBZ, &CAsMonitorDlg::OnBnClickedYsbz)
	ON_BN_CLICKED(IDC_NDBC, &CAsMonitorDlg::OnBnClickedNdbc)
	ON_BN_CLICKED(IDC_TEAM, &CAsMonitorDlg::OnBnClickedTeam)
	ON_BN_CLICKED(IDC_DRJJ, &CAsMonitorDlg::OnBnClickedDrjj)
	ON_BN_CLICKED(IDC_RELOAD, &CAsMonitorDlg::OnBnClickedReload)
	ON_BN_CLICKED(IDC_REFRESH, &CAsMonitorDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_TASK, &CAsMonitorDlg::OnBnClickedTask)
END_MESSAGE_MAP()


BOOL CAsMonitorDlg::DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR szDriveStr[500];
	TCHAR szDrive[3];
	TCHAR szDevName[100];
	INT cchDevName;
	INT i;
	//检查参数
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询Dos设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}

//获取进程完整路径
BOOL CAsMonitorDlg::GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
{
	TCHAR szImagePath[MAX_PATH];
	HANDLE hProcess;
	if (!pszFullPath)
		return FALSE;

	pszFullPath[0] = '\0';
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
	if (!hProcess)
		return FALSE;

	if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	if (!DosPathToNtPath(szImagePath, pszFullPath))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	CloseHandle(hProcess);

	_tprintf(_T("%d,%s \r\n"), dwPID, pszFullPath);
	return TRUE;
}

UINT CAsMonitorDlg::ProcThread(LPVOID lpv)
{
	CAsMonitorDlg* p = (CAsMonitorDlg*)lpv;
	p->Launch();
	return 0;
}

// CAsMonitorDlg 消息处理程序

BOOL CAsMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//GetModuleFileName(0, m_tszPath, MAX_PATH);
	//TCHAR* p = _tcsrchr(m_tszPath,'\\');
	//if (p)
	//{
	//	*p = 0;
	//	_tcscat_s(m_tszPath, TEXT("\\cefclient\\"));
	//	SetCurrentDirectory(m_tszPath);
	//}
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof nid;
	nid.hIcon = m_hIcon;
	nid.hWnd = m_hWnd;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uID = IDR_MAINFRAME;
	_tcscpy_s(nid.szTip, TEXT("AssMonitor"));
	nid.uCallbackMessage = WM_SHELLNOTIFY;
	Shell_NotifyIcon(NIM_ADD, &nid);
	//ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	m_pH5 = (CListCtrl*)GetDlgItem(IDC_H5);
	m_pH5->SetImageList(&m_ImgLst, LVSIL_NORMAL);
	m_pH5->SetIconSpacing(64, 64);
	InitList();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAsMonitorDlg::InitList()
{
#if 0
	WIN32_FIND_DATA wfd = { 0 };
	int index = 0;
	BOOL bFind = TRUE;
	HANDLE hFind = FindFirstFile(TEXT("*.ini"), &wfd);
	m_pH5->DeleteAllItems();
	while (bFind && hFind != INVALID_HANDLE_VALUE)
	{
		*wcsrchr(wfd.cFileName, '.') = 0;
		m_pH5->InsertItem(index, wfd.cFileName, 0);
		bFind = FindNextFile(hFind, &wfd);
	}
#else
	DWORD i,*dwIds = 0;
	HWND hWnd = 0;
	WCHAR szTitle[MAX_PATH] = { 0 };
	LVITEM lvitem = { 0 };
	m_pH5->DeleteAllItems();
	//i = GetProcessIdByName(TEXT("cefclient.exe"), &dwIds);
	i = GetProcessIdByOrigName(TEXT("Assistant.exe"), &dwIds);
	for (int n = 0; n < i; n++)
	{
		hWnd = GetHandleByProcessId(dwIds[n]);
		memset(szTitle, 0, sizeof(szTitle));
		::GetWindowText(hWnd, szTitle, MAX_PATH);
		if (wcslen(szTitle) == 0) continue;
		lvitem.pszText = szTitle;
		lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvitem.iImage = 1;
		lvitem.iItem = n;
		lvitem.cchTextMax = MAX_PATH;
		lvitem.lParam = (LPARAM)hWnd;
		m_pH5->InsertItem(&lvitem);
	}
#endif
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAsMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAsMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAsMonitorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	WCHAR wszCmd[260] = { 0 };

	Shell_NotifyIcon(NIM_DELETE, &nid);
/*
	LV_ITEM lvitem;
	int c = m_pH5->GetItemCount();
	for (int i = 0; i < c; i++)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = i;
		lvitem.mask = LVIF_PARAM;
		m_pH5->GetItem(&lvitem);
		HWND hWnd = (HWND)lvitem.lParam;
		if (IsWindow(hWnd))
		{
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
*/
}

LRESULT CAsMonitorDlg::ShellIconNofity(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDOWN:
		//nFlag = !nFlag;
		//if (nFlag) {
		if (!IsWindowVisible()) {
			ShowWindow(SW_SHOW);
			CenterWindow();
			//BringWindowToTop(self->hwnd_);
		}
		else ShowWindow(SW_HIDE);
		break;
	//case WM_LBUTTONDBLCLK:
	//	CDialog::OnOK();
	//	break;
	default:
		break;
	}
	return 0L;
}

BOOL CAsMonitorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::OnCommand(wParam, lParam);
}

BOOL CAsMonitorDlg::Launch()
{	
	WCHAR wszCmd[260] = { 0 };
	LV_ITEM lvitem;
	GetDlgItem(IDC_RUN)->EnableWindow(0);
	GetDlgItem(IDC_CLOSE)->EnableWindow(0);
	GetDlgItem(IDC_EXIT)->EnableWindow(0);
	GetDlgItem(IDC_HIDE)->EnableWindow(0);
	EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
	int c = m_pH5->GetItemCount();
	for (int i = 0; i < c; i++)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = i;
		lvitem.cchTextMax = 260;
		lvitem.pszText = wszCmd;
		lvitem.mask = LVIF_TEXT;
		m_pH5->GetItem(&lvitem);
		if (wcslen(wszCmd))
		{
			LaunchApp(wszCmd, i);
		}
	}
	for (int i = c-1; i >= 0; i--)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = i;
		lvitem.mask = LVIF_PARAM;
		m_pH5->GetItem(&lvitem);
		if (!lvitem.lParam)
		{
			m_pH5->DeleteItem(i);
		}
	}
	GetDlgItem(IDC_RUN)->EnableWindow();
	GetDlgItem(IDC_CLOSE)->EnableWindow();
	GetDlgItem(IDC_EXIT)->EnableWindow();
	GetDlgItem(IDC_HIDE)->EnableWindow();
	EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	return TRUE;
}

void CAsMonitorDlg::LaunchApp(LPTSTR lpAppName, int iIndex)
{
	LV_ITEM lvitem;

	HWND hWnd = GetHandleByTitle(TEXT("cefclient.exe"),lpAppName);
	if (hWnd)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = iIndex;
		lvitem.iImage = 1;
		lvitem.lParam = (LPARAM)hWnd;
		lvitem.mask = LVIF_IMAGE | LVIF_PARAM;
		m_pH5->SetItem(&lvitem);
		return;
	}
}

void CAsMonitorDlg::OnBnClickedHide()
{
	// TODO: 在此添加控件通知处理程序代码
	LV_ITEM lvitem;
	int c = m_pH5->GetItemCount();
	for (int i = 0; i < c; i++)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = i;
		lvitem.mask = LVIF_PARAM;
		m_pH5->GetItem(&lvitem);
		HWND hWnd = (HWND)lvitem.lParam;
		if (IsWindow(hWnd))
		{
			::ShowWindow(hWnd, SW_HIDE);
		}
	}
}



void CAsMonitorDlg::OnNMClickH5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LV_ITEM lvitem;
	int nItem = -1;
	if (pNMItemActivate != NULL)
	{
		nItem = pNMItemActivate->iItem;
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = nItem;
		lvitem.mask = LVIF_PARAM;
		m_pH5->GetItem(&lvitem);
		HWND hWnd = (HWND)lvitem.lParam;
		if (IsWindow(hWnd))
		{
			if (!::IsWindowVisible(hWnd))
			{
				::SetWindowPos(hWnd, 0, 20, 10, 400, 703, SWP_FRAMECHANGED);
				::ShowWindow(hWnd, SW_SHOWNORMAL);
			}
			::SetForegroundWindow(hWnd);
		}
	}
	*pResult = 0;
}


void CAsMonitorDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);
	//CDialogEx::OnClose();
}


void CAsMonitorDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}


void CAsMonitorDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	LV_ITEM lvitem;
	int c = m_pH5->GetItemCount();
	for (int i = 0; i < c; i++)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.iItem = i;
		lvitem.mask = LVIF_PARAM;
		m_pH5->GetItem(&lvitem);
		HWND hWnd = (HWND)lvitem.lParam;
		if (IsWindow(hWnd))
		{
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedRun()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF0,0);
		}
	}
}

void CAsMonitorDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF1,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedGh()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF2,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedSy()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF3,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedMj()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	UpdateData();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF4,m_nMjValue);
		}
	}
}


void CAsMonitorDlg::OnBnClickedZbtx()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF5,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedMld()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF6,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedMlx()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF7,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedByfl()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF8,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedTtsd()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xF9,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedYsbz()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFA,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedNdbc()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFB,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedTeam()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	UpdateData();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFC,m_nTeamValue);
		}
	}
}


void CAsMonitorDlg::OnBnClickedDrjj()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	UpdateData();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFD,m_nDrValue);
		}
	}
}


void CAsMonitorDlg::OnBnClickedReload()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFE,0);
		}
	}
}


void CAsMonitorDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	InitList();
}


void CAsMonitorDlg::OnBnClickedTask()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_pH5->GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_pH5->GetNextSelectedItem(pos);
		LVITEM lv = { 0 };
		lv.iItem = nItem;
		lv.mask = LVIF_PARAM;
		m_pH5->GetItem(&lv);
		if (lv.lParam)
		{
			::SendMessage((HWND)lv.lParam, WM_CUSTOMFUN, 0xFF,0);
		}
	}
}
