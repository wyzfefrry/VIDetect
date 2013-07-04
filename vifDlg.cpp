// vifDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "vif.h"
#include "vifDlg.h"
#include "SkinH.h"
#include "Config.h"
#include "highgui.h"
#include "WinInet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CvifDlg::CvifDlg(CWnd* pParent /*=NULL*/)
: CDialog(CvifDlg::IDD, pParent)
, m_ipAddr(_T("172.168.1.101"))
, m_userName(_T("admin"))
, m_passWord(_T(""))
, m_Port(_T("3645"))
, m_bFlag(TRUE)
{
	memset(&m_threadInfo, 0, sizeof(m_threadInfo));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvifDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBOTARGET, m_cboTarget);
	DDX_Control(pDX, IDC_CBOSOURCE, m_cboSource);
	DDX_Text(pDX, IDC_IPADDR, m_ipAddr);
	DDV_MaxChars(pDX, m_ipAddr,15);
	DDX_Text(pDX, IDC_USERNAME, m_userName);
	DDX_Text(pDX, IDC_PSW, m_passWord);
	DDX_Text(pDX, IDC_PORT, m_Port);
}

BEGIN_MESSAGE_MAP(CvifDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MbGreen_MAP
	ON_BN_CLICKED(IDC_BTNEXECUTE, &CvifDlg::OnBtnExecute)
	ON_BN_CLICKED(IDC_BTNBROWSER, &CvifDlg::OnBtnBrowser)
	ON_BN_CLICKED(IDC_BTNNEXT, &CvifDlg::OnBtnNext)
	ON_CBN_CLOSEUP(IDC_CBOSOURCE, &CvifDlg::OnCloseUpCboSource)
	ON_CBN_CLOSEUP(IDC_CBOTARGET, &CvifDlg::OnCloseUpCboTarget)
	ON_BN_CLICKED(IDC_BTNMATCH, &CvifDlg::OnBtnMatch)
	ON_BN_CLICKED(IDC_BTNRECORD, &CvifDlg::OnBtnRecord)
	ON_BN_CLICKED(IDC_BUTTON5, &CvifDlg::OnBtnLogin)
	ON_BN_CLICKED(IDC_BUTTON3, &CvifDlg::OnBtnPlaying)
	ON_BN_CLICKED(IDC_BUTTON7, &CvifDlg::OnBtnLogout)
	ON_BN_CLICKED(IDC_BUTTON6, &CvifDlg::OnBtnStop)
	ON_BN_CLICKED(IDC_BUTTON8, &CvifDlg::OnBtnVIDetect)
END_MESSAGE_MAP()

// CvifDlg 消息处理程序
BOOL CvifDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);	
	setlocale (LC_ALL, "chs" );
	SkinH_Attach();
	SetWindowText(_T("VIF - 安联烟火识别系统"));
 
	m_MainMenu.LoadMenu(IDR_MAINFRAME);
	SetMenu(&m_MainMenu);

	CString szFilePath;
	GetDlgItemText(IDC_EDITFILEPATH,szFilePath);
	if (szFilePath.IsEmpty()) {
		CVIFApp *theApp = (CVIFApp *)AfxGetApp();;
		CString szFilePath = theApp->GetRootPath()+_T("\\data\\new.avi");
		SetDlgItemText(IDC_EDITFILEPATH,szFilePath);
	}
	return TRUE;  
}

void CvifDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CvifDlg::OnPaint()
{
	OnCloseUpCboSource();
	OnCloseUpCboTarget();

	if (IsIconic()) {
		CPaintDC dc(this); 
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

HCURSOR CvifDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CvifDlg::OnBtnExecute()
{
	CString szFilePath;
	GetDlgItemText(IDC_EDITFILEPATH,szFilePath);
	if (szFilePath.IsEmpty()) {
		AfxMessageBox(_T("文件不存在!"));
		return; 
	}

	char *sTargetFile = new char[szFilePath.GetLength()+1];
	WCHAR *wsText = (WCHAR *)szFilePath.GetBuffer();
	ws2as(wsText,sTargetFile,lstrlen(wsText)+1);

	CvCapture* capture = cvCreateFileCapture(sTargetFile);
	szFilePath.ReleaseBuffer();
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS );
	delete []sTargetFile;

	if(capture == NULL) {
		AfxMessageBox(_T("无法识别的视频文件!"));
		return;
	};    

	int key = 0;
	IplImage* frame = NULL;
	cvNamedWindow("vifVideo", CV_WINDOW_AUTOSIZE);
	while(1) {
		frame = cvQueryFrame(capture);
		if (!frame) break;
		cvShowImage("vifVideo", frame);
		key = cvWaitKey(33);
		if( key == 27 ) break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("vifVideo"); 
}

void CvifDlg::OnBtnBrowser()
{
	CVIFApp *theApp = (CVIFApp *)AfxGetApp();
	TRACE(_T("%s"),theApp->GetRootPath());
	CString szFilePath = theApp->GetRootPath()+_T("\\data");
	
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL);
	dlg.m_ofn.lpstrInitialDir = szFilePath.GetBuffer();
	dlg.m_ofn.lpstrFilter = _T("avi Files (*.avi)|*.avi|mpeg files(*.mpeg)|*mpeg||");
	szFilePath.ReleaseBuffer();
	if (IDOK == dlg.DoModal()) {
		SetDlgItemText(IDC_EDITFILEPATH,dlg.GetFileName());
	}
}

//void CvifDlg::OnBtnPlay()
//{
//	CString szFilePath;
//	GetDlgItemText(IDC_EDITFILEPATH,szFilePath);
//	if (szFilePath.IsEmpty()) {
//		AfxMessageBox(_T("文件不存在!"));
//		return; 
//	}
//
//	char *sTargetFile = new char[szFilePath.GetLength()+1];
//	WCHAR *wsText = (WCHAR *)szFilePath.GetBuffer();
//	ws2as(wsText,sTargetFile,lstrlen(wsText)+1);
//	
//	bool bStatus = m_VideoStream.Open(sTargetFile);
//	delete []sTargetFile;
//	if (!bStatus) {
//		AfxMessageBox(_T("无法识别的视频文件!"));
//		return;
//	}
//	
//	m_cboSource.ResetContent();
//	m_cboTarget.ResetContent();
//	int i;
//	for (i = 0; i < m_VideoStream.GetFrameCount(); i++) {
//		CString szText;
//		szText.Format(_T("%d"),i);
//		m_cboSource.AddString(szText);
//		m_cboTarget.AddString(szText);
//	}
//
//	if (m_VideoStream.GetFrameCount() > 1) {
//		m_cboSource.SetCurSel(0);
//		m_cboTarget.SetCurSel(1);
//	}
//	
//	OnCloseUpCboSource();
//	OnCloseUpCboTarget();
//}

void CvifDlg::OnBtnNext()
{
	if (!m_VideoStream.IsOpen() || m_cboSource.GetCurSel() < 0 
		|| m_cboTarget.GetCurSel() < 0) {
		return;
	}

	if (m_cboSource.GetCurSel()+1 < m_cboSource.GetCount() 
		&& m_cboTarget.GetCurSel()+1 < m_cboTarget.GetCount()) {
		m_cboSource.SetCurSel(m_cboSource.GetCurSel()+1);
		m_cboTarget.SetCurSel(m_cboTarget.GetCurSel()+1);
	}

	OnCloseUpCboSource();
	OnCloseUpCboTarget();
}

void CvifDlg::OnCloseUpCboSource()
{
	SetTimer(HR_TIMEPALY1,VW_TIMEMILLISECONDS,NULL);
	OnTimer(HR_TIMEPALY1);
}

void CvifDlg::OnCloseUpCboTarget()
{
	SetTimer(HR_TIMEPALY2,VW_TIMEMILLISECONDS,NULL);
	OnTimer(HR_TIMEPALY2);
}

void CvifDlg::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == HR_TIMEPALY1) {
		GetDlgItem(IDC_PICCAM2)->Invalidate();
		if (m_cboSource.GetCurSel() > -1) {

			IplImage* pFrame = m_VideoStream.GetFrame(m_cboSource.GetCurSel());
			CDC *pDC =  GetDlgItem(IDC_PICCAM2)->GetDC();
			CRect rcClient;
			GetDlgItem(IDC_PICCAM2)->GetClientRect(&rcClient);
			CvvImage vImage;
			vImage.CopyOf(pFrame);
			vImage.DrawToHDC(pDC->m_hDC,&rcClient);
			ReleaseDC(pDC);

			//pDC =  GetDlgItem(IDC_PICCAM1)->GetDC();
			//GetDlgItem(IDC_PICCAM1)->GetClientRect(&rcClient);
			//vImage.CopyOf(pFrame);
			//vImage.DrawToHDC(pDC->m_hDC,&rcClient);
			//ReleaseDC(pDC);
		}
	} else if (nIDEvent == HR_TIMEPALY2) {
		GetDlgItem(IDC_PICCAM3)->Invalidate();
		if (m_cboTarget.GetCurSel() > -1) {

			IplImage* pFrame = m_VideoStream.GetFrame(m_cboTarget.GetCurSel());
			CDC *pDC = GetDlgItem(IDC_PICCAM3)->GetDC();
			CRect rcClient;
			GetDlgItem(IDC_PICCAM3)->GetClientRect(&rcClient);
			CvvImage vImage;
			vImage.CopyOf(pFrame);
			vImage.DrawToHDC(pDC->m_hDC,&rcClient);
			ReleaseDC(pDC);

			//pDC =  GetDlgItem(IDC_PICCAM4)->GetDC();
			//GetDlgItem(IDC_PICCAM4)->GetClientRect(&rcClient);
			//vImage.CopyOf(pFrame);
			//vImage.DrawToHDC(pDC->m_hDC,&rcClient);
			//ReleaseDC(pDC);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CvifDlg::OnBtnMatch()
{
	if (!m_VideoStream.IsOpen() || m_cboSource.GetCurSel() < 0 
		|| m_cboTarget.GetCurSel() < 0) {
		return;
	}
	IplImage* pImage = m_VideoStream.GetFrame(m_cboSource.GetCurSel());
	VI_FRAME vibRedcFrame, viTarFrame;
	CSystemHelper::CopyFrame(&vibRedcFrame,pImage);
	m_FrameMatcher.SetTemplate(&vibRedcFrame);

	pImage = m_VideoStream.GetFrame(m_cboTarget.GetCurSel());
	CSystemHelper::CopyFrame(&viTarFrame,pImage);

	//两帧匹配方法 参数一：目标帧  参数二：调用的子方法索引（1.级数 2.方差 3.中心系数1.5 其他.为NULL）
	VI_COMMUNITY* pRedRegionGrid = LPVI_COMMUNITY(m_FrameMatcher.Match(&viTarFrame, 1));
	if(pRedRegionGrid == NULL) {
		return;
	}
	/*
	//目标帧复制
	IplImage* pImageTest = new IplImage();

	pImageTest->dataOrder = viTarFrame.dataOrder;
	pImageTest->depth = viTarFrame.depth;
	pImageTest->height = pRedRegionGrid->nHeigh;
	pImageTest->width = pRedRegionGrid->nWidth;
	pImageTest->ID = viTarFrame.ID;
	pImageTest->nChannels = viTarFrame.nChannels;
	pImageTest->widthStep =  pRedRegionGrid->nWidth * viTarFrame.nChannels;
	pImageTest->nSize = viTarFrame.nSize;
	pImageTest->origin = viTarFrame.origin;
	pImageTest->roi = NULL;
	pImageTest->maskROI = NULL;
	pImageTest->imageData = new char[pImageTest->widthStep * pImageTest->height + 1];
	for(int j = 0; j < pImageTest->height; j++){
		for(int i = 0; i < pImageTest->widthStep; i++){
			pImageTest->imageData[j * pImageTest->widthStep + i] = viTarFrame.imageData[(j + pRedRegionGrid->nTarTop) * viTarFrame.widthStep + (i + pRedRegionGrid->nTarLeft * viTarFrame.nChannels)];
		}
	}
	*/
	IplImage* pImageTest = CSystemHelper::CreateSubImage(
		&viTarFrame, pRedRegionGrid->nTarLeft, pRedRegionGrid->nTarTop, 
		pRedRegionGrid->nWidth, pRedRegionGrid->nHeigh
	);
	CDC *pDC = GetDlgItem(IDC_PICCAM4)->GetDC();
	CRect rcClient;
	GetDlgItem(IDC_PICCAM4)->GetClientRect(&rcClient);
	CvvImage vImage;
	vImage.CopyOf(pImageTest);
	vImage.DrawToHDC(pDC->m_hDC,&rcClient);
	ReleaseDC(pDC);

	//模板帧
	IplImage* pImageTemplate = new IplImage;

	pImageTemplate->dataOrder = vibRedcFrame.dataOrder;
	pImageTemplate->depth = vibRedcFrame.depth;
	pImageTemplate->height = pRedRegionGrid->nHeigh;
	pImageTemplate->width = pRedRegionGrid->nWidth;
	pImageTemplate->ID = vibRedcFrame.ID;
	pImageTemplate->nChannels = vibRedcFrame.nChannels;
	pImageTemplate->widthStep =  pRedRegionGrid->nWidth * vibRedcFrame.nChannels;
	pImageTemplate->nSize = vibRedcFrame.nSize;
	pImageTemplate->origin = vibRedcFrame.origin;
	pImageTemplate->imageData = new char[pImageTemplate->widthStep * pImageTemplate->height + 1];
	for(int j = 0; j < pImageTemplate->height; j++){
		for(int i = 0; i < pImageTemplate->widthStep; i++){
			pImageTemplate->imageData[j * pImageTemplate->widthStep + i] = vibRedcFrame.imageData[(j + pRedRegionGrid->nTempTop) * vibRedcFrame.widthStep + (i + pRedRegionGrid->nTempLeft * vibRedcFrame.nChannels)];
		}
	}
	pDC = GetDlgItem(IDC_PICCAM1)->GetDC();
	GetDlgItem(IDC_PICCAM1)->GetClientRect(&rcClient);
	vImage.CopyOf(pImageTemplate);
	vImage.DrawToHDC(pDC->m_hDC,&rcClient);
	ReleaseDC(pDC);

	//在共同区域中输出不同区域 用第五个显示器
	LPVI_CELL m_pChangeCell = LPVI_CELL(m_FrameMatcher.Change());

	for(int i = 0; i < (pImageTest->width / m_FrameMatcher.GetCellSize()) * (pImageTest->height / m_FrameMatcher.GetCellSize()); i++){
		// 把不同区域用蓝色点标出
		if(m_pChangeCell[i].dHSV == 0){
			for(int w = 0; w < m_FrameMatcher.GetCellSize(); w++){
				for(int v = 0; v < m_FrameMatcher.GetCellSize(); v++){
					BYTE* ptr = (BYTE*)(pImageTest->imageData + (m_pChangeCell[i].nCellHeigh + w) * pImageTest->widthStep);
					ptr[m_FrameMatcher.GetCellSize() * (m_pChangeCell[i].nCellWidth + v)] = 255;
					ptr[m_FrameMatcher.GetCellSize() * (m_pChangeCell[i].nCellWidth + v) + 1] = 0;
					ptr[m_FrameMatcher.GetCellSize() * (m_pChangeCell[i].nCellWidth+ v) + 2] = 0;
				}
			}
		}
	}
	pDC = GetDlgItem(IDC_PICCAM5)->GetDC();
	GetDlgItem(IDC_PICCAM5)->GetClientRect(&rcClient);
	vImage.CopyOf(pImageTest);
	vImage.DrawToHDC(pDC->m_hDC,&rcClient);
	ReleaseDC(pDC);

	CSystemHelper::ZeroFrame(&vibRedcFrame);
	CSystemHelper::ZeroFrame(&viTarFrame);
	CSystemHelper::ZeroImage(pImageTest);
	CSystemHelper::ZeroImage(pImageTemplate);

	delete pImageTest;
	delete pImageTemplate;
}

void CvifDlg::OnBtnRecord()
{
	CVIFApp *theApp = (CVIFApp*)AfxGetApp();
	CConfig *pConfig = theApp->GetSystemConfig();
	TCHAR sValue[VIN_VALUEPROPERTYLENGTH];
	pConfig->GetProperty(_T("app"), _T("windowcount"), sValue, _T(""));
	AfxMessageBox(sValue);
}

CString szUsername;
CString szPassword;
CString	szIpAdr;
int		nPort;

void CvifDlg::OnBtnLogin() {
	UpdateData(TRUE);
	CString	szPort;
	GetDlgItemText(IDC_USERNAME, szUsername);
	GetDlgItemText(IDC_PSW, szPassword);
	GetDlgItemText(IDC_IPADDR, szIpAdr);
	GetDlgItemText(IDC_PORT, szPort);
	nPort = _ttoi(szPort);
	if (_tcscmp(szUsername, _T("admin")) != 0 || _tcscmp(szPassword,_T("")) != 0) {
		AfxMessageBox(_T("登录失败：用户名或密码不对"));
		return ;
	}
	if (nPort != 3645 || _tcscmp(szIpAdr, _T("172.168.1.101")) != 0) {
		AfxMessageBox(_T("登录失败：请输入 正确的视频服务器IP及端口"));
		return ;
	}
	m_NrcServer.Login(szIpAdr, nPort, szUsername,szPassword);
}

HWND hWnd = NULL;
void CvifDlg::OnBtnPlaying() {
	CString szInfo;
	CWnd *pWnd = GetDlgItem(IDC_PICCAM5); 
	if (pWnd != NULL) {
		hWnd = pWnd->GetSafeHwnd();
	}
	if (pWnd != NULL) {
		m_NrcServer.StartPlay(" ", hWnd);
	}
}

void CvifDlg::OnBtnLogout() {
	m_NrcServer.Logout();
}

void CvifDlg::OnBtnStop() {
	m_NrcServer.StopPlay(" ");
}

UINT CvifDlg::Detect(LPVOID pParam) {
	LPTHREADINFO lpThreadInfo = (LPTHREADINFO)pParam;
	lpThreadInfo->detectClass->Register(lpThreadInfo->serverClass);
	lpThreadInfo->detectClass->StartDetect();
	return 0;
}

void CvifDlg::AddDetectServer() {
	m_threadInfo[0].serverClass = &m_NrcServer;
	m_threadInfo[0].detectClass = &m_VIDetect;
}

void CvifDlg::OnBtnVIDetect() {
	if (m_bFlag == TRUE) {
		m_bFlag = FALSE;
		AddDetectServer();
		AfxBeginThread(Detect,&m_threadInfo[0]);
	}
}
