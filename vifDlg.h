// vifDlg.h : ͷ�ļ�
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

typedef struct _THREADINFO{
	CNrcServer* serverClass;
	VIDetect*  detectClass;
}THREADINFO, *LPTHREADINFO;

// CvifDlg �Ի���
class CvifDlg : public CDialog
{
// ����
public:
	CvifDlg(CWnd* pParent = NULL);	// ��׼���캯��
	THREADINFO threadInfo;
// �Ի�������
	enum { IDD = IDD_VIF_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnExecute();
	afx_msg void OnBtnBrowser();
	afx_msg void OnBtnNext();
	afx_msg void OnCloseUpCboSource();
	afx_msg void OnCloseUpCboTarget();
	afx_msg void OnBtnMatch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnBtnRecord();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnLogin();
	afx_msg void OnBtnPlaying();
	afx_msg void OnBtnLogout();
	afx_msg void OnBtnStop();
public:
	static UINT	Vetect(LPVOID pParam);
private:
	CVideoStream	    m_VideoStream;
	CFrameMatcher	    m_FrameMatcher;
	CNrcServer			m_NrcServer;
	VIDetect		    m_VIDetect;
protected:
	CComboBox	m_cboTarget;
	CComboBox	m_cboSource;
	CMenu		m_MainMenu;
public:
	afx_msg void OnBtnVIDetect();
	CString		m_ipAddr;
	CString		m_userName;
	CString		m_passWord;
	CString		m_nPort;
	BOOL		m_bFlag;

};


#endif //_VIFDLG_H_