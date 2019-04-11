
// AsMonitorDlg.h : 头文件
//

#pragma once
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

#define WM_SHELLNOTIFY	WM_USER+101
#define WM_CUSTOMFUN	WM_USER+102
typedef struct _WND_INFO
{
	HWND hWnd;
	DWORD dwProcessId;
}WNDINFO, *PWNDINFO;

typedef struct _tagNode
{
	int iIndex;
	HWND hWnd;
	HANDLE hProcess;
	DWORD dwThreadId;
}Node;

// CAsMonitorDlg 对话框
class CAsMonitorDlg : public CDialogEx
{
// 构造
public:
	CAsMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASMONITOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
private:
	static UINT ProcThread(LPVOID lpv);
	void InitList();
	BOOL Launch();
	void LaunchApp(LPTSTR lpAppName,int iIndex);
	LPTSTR GetProcessNameById(DWORD dwProcessId);
	DWORD GetProcessIdByName(LPTSTR lpszName);
	DWORD GetProcessIdByName(LPTSTR lpszName, LPDWORD* lpIds);
	DWORD GetProcessIdByOrigName(LPTSTR lpszName, LPDWORD* lpIds);
	HWND GetHandleByTitle(LPTSTR lpProcessName, LPTSTR lpszName);
	HWND GetHandleByProcessId(DWORD dwProcessId);
	BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath);
	BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH]);
protected:
	HICON m_hIcon;
	NOTIFYICONDATA nid;
	CListCtrl* m_pH5;
	CImageList m_ImgLst;
	CCriticalSection m_cs;
	TCHAR m_tszPath[MAX_PATH];

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg LRESULT ShellIconNofity(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedHide();
	afx_msg void OnNMClickH5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedGh();
	afx_msg void OnBnClickedSy();
	afx_msg void OnBnClickedMj();
	afx_msg void OnBnClickedZbtx();
	afx_msg void OnBnClickedMld();
	afx_msg void OnBnClickedMlx();
	afx_msg void OnBnClickedByfl();
	afx_msg void OnBnClickedTtsd();
	afx_msg void OnBnClickedYsbz();
	afx_msg void OnBnClickedNdbc();
	afx_msg void OnBnClickedTeam();
	afx_msg void OnBnClickedDrjj();
	afx_msg void OnBnClickedReload();
	afx_msg void OnBnClickedRefresh();
	UINT m_nMjValue;
	UINT m_nTeamValue;
	UINT m_nDrValue;
	afx_msg void OnBnClickedTask();
};
