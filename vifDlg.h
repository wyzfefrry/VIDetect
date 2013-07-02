// vifDlg.h : 头文件
//

#ifndef _VIFDLG_H_
#define _VIFDLG_H_

#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include "utils.h"
#include "VideoStream.h"
#include "FrameMatcher.h"
#include "SystemHelper.h"
#include "afxwin.h"
#include "CNrcServer.h"
#include "VIDetect.h"

#define VW_TIMEMILLISECONDS		30000
#define HR_TIMEPALY1			2991
#define HR_TIMEPALY2			2992
#define SERVER_NUM				2
typedef struct _THREADINFO{
	CNrcServer* serverClass;
	VIDetect*  detectClass;
}THREADINFO, *LPTHREADINFO;

// CvifDlg 对话框
class CvifDlg : public CDialog
{
// 构造
public:
	CvifDlg(CWnd* pParent = NULL);	// 标准构造函数
	// 对话框数据
	enum { IDD = IDD_VIF_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnBtnExecute();
	afx_msg void	OnBtnBrowser();
	afx_msg void	OnBtnNext();
	afx_msg void	OnCloseUpCboSource();
	afx_msg void	OnCloseUpCboTarget();
	afx_msg void	OnBtnMatch();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void	OnPaint();
	afx_msg void	OnBtnRecord();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void	OnBtnLogin();
	afx_msg void	OnBtnPlaying();
	afx_msg void	OnBtnLogout();
	afx_msg void	OnBtnStop();
	afx_msg void	OnBtnVIDetect();
public:
	//检测服务器状态
	static UINT	Detect(LPVOID pParam);
	////增加需要检测的服务器
	void AddDetectServer();
private:
	CVideoStream	    m_VideoStream;
	CFrameMatcher	    m_FrameMatcher;
	CNrcServer			m_NrcServer;
	VIDetect		    m_VIDetect;
	enum				{m_serverNum = 1};
protected:
	CComboBox			m_cboTarget;
	CComboBox			m_cboSource;
	CMenu				m_MainMenu;
public:
	CString				m_ipAddr;
	CString				m_userName;
	CString				m_passWord;
	CString				m_szPort;
	BOOL				m_bFlag;
	THREADINFO			m_threadInfo[m_serverNum];
};

#endif //_VIFDLG_H_