#include "stdafx.h"
#include "WinInet.h"
#include "windows.h"
#include "CNrcServer.h"
#include "afxmt.h"
#include "utils.h"
#include <windows.h>

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
CEvent	netRecover(FALSE,FALSE);

UINT CNrcServer::NetDetectRecover(LPVOID pParam) {
	CNrcServer* pDlg = (CNrcServer*)pParam;
	while(1) {
		netRecover.Lock();
		if (pDlg->m_bNetworkFlag == FALSE) {
			pDlg->m_bNetWorkstate = TRUE;
			pDlg->m_bNetworkFlag  = TRUE;
			pDlg->m_bThread = TRUE;
		}
	}
}

UINT CNrcServer::Run(LPVOID pParam) {
	CNrcServer* pDlg = (CNrcServer*)pParam;
	while(1) {
		DWORD currentTime = GetTickCount();	
		signed int intervalTime = currentTime - pDlg->m_dwLastPlayingTime;
		if (intervalTime > 5000) {
			pDlg->m_bNetWorkstate = FALSE;
			Sleep(1000);
			pDlg->m_bNetworkFlag = FALSE;
			AfxEndThread(0);
		}
	}
	return 0;
}

//事件的回调函数
int __stdcall EventRecvCallBack(UINT uiSession, LPCNcEventInfo pEventInfo, int nErrorCode, void *context)
 {
	CNrcServer *pDlg = (CNrcServer*)context;
	if (pEventInfo->uiEventID == 0x30001) {
		pDlg->m_bServerState = FALSE;	
	} else if (pEventInfo->uiEventID == 0x30002) {
		pDlg->m_bServerState = TRUE;
	}
	return 0;
}

void CNrcServer::StartThread() {
	AfxBeginThread(Run, this);
}

int __stdcall StreamReadCallBack(UINT uiSession, NcGUID guid, StreamType streamType, 
								 TransferType transferType, BYTE *pData, UINT uiLength, 
								 LPCNcFrameInfo pFramInfo, int nErrorCode, void *context) {
	CNrcServer *pDlg = (CNrcServer*)context;
	if (pDlg->m_bThread == TRUE) {
		pDlg->StartThread();
		pDlg->m_bThread = FALSE;
	}
	//发送通知信号 通知网络恢复
	netRecover.SetEvent();		
	pDlg->m_dwLastPlayingTime = GetTickCount();
	BYTE *pGUID = (BYTE *)guid;
	int rv = 0;
	wchar_t szPath[1024];
	GetModuleFileName(NULL, szPath, 1024);
	wchar_t *p = wcsrchr(szPath, '\\');
	*p = '\0';
	if (pFramInfo->ver == frame_video)
	{
#ifdef _OUTPUT_FRAME_TIMESTAMP
		TRACE("video frame %5d %10d %3d\n", uiLength, 
			pFramInfo->video.timeStamp, pFramInfo->video.timeStamp - g_uiLastVideo);
		g_uiLastVideo = pFramInfo->video.timeStamp;
#endif // _OUTPUT_FRAME_TIMESTAMP
		//向媒体解码库中加入一帧视、音频数据
		rv = VADR_PumpVideoFrame(pDlg->m_hVARender, pData, uiLength, 
			pFramInfo->video.timeStamp, pFramInfo->video.alg,
			pFramInfo->video.flag.iframe, FALSE);
		rv = VAS_PumpVideoFrame(pDlg->m_hVAStorage, pData, uiLength, 
			pFramInfo->video.timeStamp, pFramInfo->video.alg, 
			pFramInfo->video.flag.iframe, 
			pFramInfo->video.width * 8, pFramInfo->video.height * 8);
			CString szFile;
			szFile.Format(_T("%s\\%s"), szPath, "Video.bin");
			FILE *fp = fopen((const char*)(LPCTSTR)szFile, "ab+");
			fwrite(&uiLength, sizeof(UINT), 2, fp);
			fwrite(pData, uiLength, 1, fp);
			fclose(fp);
	}
	else if (pFramInfo->ver == frame_audio)
	{
#ifdef _OUTPUT_FRAME_TIMESTAMP
		TRACE("audio frame %5d %10d %3d\n", uiLength, 
			pFramInfo->audio.timeStamp, pFramInfo->audio.timeStamp - g_uiLastAudio);
		g_uiLastAudio = pFramInfo->audio.timeStamp;
#endif // _OUTPUT_FRAME_TIMESTAMP
		rv = VADR_PumpAudioFrame(pDlg->m_hVARender, pData, uiLength, 
			pFramInfo->audio.timeStamp, pFramInfo->audio.alg);
		rv = VAS_PumpAudioFrame(pDlg->m_hVAStorage, pData, uiLength, 
			pFramInfo->audio.timeStamp, pFramInfo->audio.alg);
		CString szFile;
		szFile.Format(_T("%s\\%s"), szPath, "Video.bin");
		FILE *fp = fopen((const char*)(LPCTSTR)szFile, "ab+");	
		fwrite(&uiLength, sizeof(UINT), 1, fp);
		fwrite(pData, uiLength, 1, fp);
		fclose(fp);
	}
	return rv;
}

extern CString	szUsername;
extern CString	szPassword;
extern CString	szIpAdr;
extern int		nPort;
extern HWND		hWnd;
#pragma comment(lib,"Wininet.lib")

UINT CNrcServer::LocalNetDetect(LPVOID pParam) {
	CNrcServer *pDlg = (CNrcServer *)pParam;
	DWORD dwFlag;
	CString szInfo;
	BOOL bFlag = TRUE;
	while(1) {
		if (InternetGetConnectedState(&dwFlag,0) == TRUE && bFlag == FALSE ) {
			pDlg->Logout();
			if (pDlg->Login(szIpAdr, nPort, szUsername,szPassword)) {
				pDlg->StartPlay(" ", hWnd);
				bFlag = TRUE;
			}
		} else if (InternetGetConnectedState(&dwFlag,0) == FALSE) {
			bFlag = FALSE;
		}
		Sleep(1000);
	}
}

void __stdcall DrawFun(HDC hDC, void *context)
{
	CNrcServer *pDlg = (CNrcServer *)context;
	CDC dc;
	if (dc.Attach(hDC))
	{
		// 写文字
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(255, 0, 0));

		CFont ft;
		ft.CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("SimSun"));

		CFont *pFont = dc.SelectObject(&ft);
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		CString szInfo;
		szInfo.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		dc.DrawText(szInfo, CRect(10, 10, 150, 100), 
					DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.SelectObject(pFont);
		ft.DeleteObject();
		dc.Detach();
	}
}


int __stdcall VideoDecode(UCHAR *pImg[3], UINT ImgWidth, UINT ImgHeight, UINT uiTimeStamp, void *pContext)
{
		CNrcServer* pDlg = (CNrcServer*)pContext;
	// 在这里可以直接将RGB数据存成BMP文件
	// 每20秒存一副图片
	if (uiTimeStamp - pDlg->m_dwLastTime > 600 * 1000)
	{
		pDlg->m_dwLastTime = uiTimeStamp;
		char szFile[MAX_PATH];
		sprintf(szFile, "0x%08X.bmp", uiTimeStamp);

		short res1 = 0;
		short res2 = 0;
		long pixoff = 54;
		long compression = 0;
		long cmpsize = 0;
		long colors = 0;
		long impcol = 0;
		char m1 = 'B';
		char m2 = 'M';
		DWORD widthDW = WIDTHBYTES(ImgWidth * 24);
		long bmfsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) 
			+ ImgWidth * ImgHeight;	
		long byteswritten=0;
		BITMAPINFOHEADER header;
		header.biSize = 40; 						// header size
		header.biWidth = ImgWidth;
		header.biHeight = ImgHeight;
		header.biPlanes = 1;
		header.biBitCount = 24;					// RGB encoded, 24 bit
		header.biCompression = BI_RGB;			// no compression
		header.biSizeImage = 0;
		header.biXPelsPerMeter = 0;
		header.biYPelsPerMeter = 0;
		header.biClrUsed = 0;
		header.biClrImportant = 0;
		
		FILE *fp = fopen(szFile, "wb");
		if (fp != NULL)
		{
			// should probably check for write errors here...
			fwrite((BYTE *)&(m1), 1, 1, fp); 
			byteswritten += 1;
			fwrite((BYTE *)&(m2), 1, 1, fp); 
			byteswritten += 1;
			fwrite((long *)&(bmfsize), 4, 1, fp);
			byteswritten += 4;
			fwrite((int *)&(res1), 2, 1, fp);
			byteswritten += 2;
			fwrite((int *)&(res2), 2, 1, fp); 
			byteswritten += 2;
			fwrite((long *)&(pixoff), 4, 1, fp); 
			byteswritten += 4;

			fwrite((BITMAPINFOHEADER *)&header, sizeof(BITMAPINFOHEADER), 1, fp);
			byteswritten += sizeof(BITMAPINFOHEADER);

			long row = 0;
			long rowidx;
			long row_size;
			row_size = header.biWidth * 3;
			long rc;
			for (row = 0; row < header.biHeight; row++) 
			{
				rowidx = (unsigned long)(row * row_size);						      

				// write a row
				rc = fwrite((void *)(pImg[0] + rowidx), row_size, 1, fp);
				if (rc != 1) 
				{
					break;
				}
				byteswritten += row_size;	
				
				// pad to DWORD
				for (DWORD count = row_size; count < widthDW; count++) 
				{
					char dummy = 0;
					fwrite(&dummy, 1, 1, fp);
					byteswritten++;
				}
			}
			fclose(fp);
		}
	}
	return 0;
}

//构造函数
CNrcServer::CNrcServer():
	m_hVARender(NULL),
	m_uiSession(NRCAP_INVALID_SESSION),
	m_bPlaying(FALSE),
	m_st(stream_realtime),
	m_tt(transfer_tcp),
	m_bInitlized(FALSE),
	m_szServerName(_T("NrcServer")),
	m_bThread(FALSE),
	m_bNetWorkstate(TRUE),
	m_bServerState(TRUE),
	m_bNetworkFlag(TRUE),
	m_dwLastTime(0) {
}

CNrcServer::~CNrcServer(){
	CString szInfo;
	if (m_uiSession != NRCAP_INVALID_SESSION) {
		NcClose(m_uiSession); 
		m_uiSession = NRCAP_INVALID_SESSION;
	}
};

//登录视频服务器
BOOL CNrcServer::Login(LPCTSTR lpszAddress, int nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword) {
	CString szInfo;
	if (m_uiSession != NRCAP_INVALID_SESSION) {
		Logout();
		ASSERT(m_uiSession == NRCAP_INVALID_SESSION);
	}
	int rv = 0;
	// 初始化协议栈
	if (m_bInitlized == FALSE) {
		rv = NcInitialize(128);
		m_bInitlized = TRUE;
	}
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Initialize Nrcap failure\n"));
		return FALSE;
	}

	// 注册截取音视频数据的回调函数
	rv = NcRegisterStreamReadCallback(StreamReadCallBack, this);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Register StreamReadCallBack failure.\n"));
		return FALSE;
	}

	// 注册接收事件的回调函数
	rv = NcRegisterEventReceiveCallback(EventRecvCallBack, this);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Register EventRecvCallBack failure.\n"));
		return FALSE;
	}

	//初始化解码库
	m_hVARender = VADR_Init(700, 600, 0);
	if (m_hVARender == NULL) {
		slog(_T("VADR_Init faiure.\n"));
		return FALSE;
	}
	CString szAddress;
	szAddress.Format(_T("%s:%d"), lpszAddress, nPort);
	char lpAddress[32];
	char lpUserName[16];
	char lpPassword[16];
	WideCharToMultiByte( CP_ACP, 0, szAddress, -1, lpAddress, 32, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, lpszUserName, -1, lpUserName, 32, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, lpszPassword, -1, lpPassword, 32, NULL, NULL );
	rv = NcOpen(&m_uiSession, (LPCTSTR)lpAddress, (LPCTSTR)lpUserName, (LPCTSTR)lpPassword, clt_resourceuser);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("NcOpen: failure.\n"));
		return FALSE;
	}

	// 开始接收服务器上的事件
	rv = NcStartEventCapture(m_uiSession);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("NcStartEventCapture: failure. \n"));
	}

	//注册视频解码回调函数
	rv = VADR_SetVideoDecodeCallBack(m_hVARender, VideoDecode, FRAMEFMT_RGB24, this);
	if (rv != VADR_OK) {
		slog(_T("VADR_SetVideoDecodeCallBack failure.\n"));
		return FALSE;
	}	

	// 注册画图回调函数  使用媒体库提供的device context在客户区播放窗口画图或写字；
	rv = VADR_SetDrawCallBack(m_hVARender, DrawFun, this);
	if (rv != VADR_OK) {
		slog(_T("VADR_SetDrawCallBack failure.\n"));
		return FALSE;
	}

	// 初始化存盘库
	m_hVAStorage = VAS_Init(700, 600);
	if (m_hVAStorage == NULL) {
		slog(_T("VAS_Init: failure.\n"));
	}
	LPVOID m_pChannel = LPVOID(1);
	return TRUE;
}

//播放视屏
BOOL CNrcServer::StartPlay(LPVOID pChannel, HWND hWnd) {
	int rv = 1;
	if (m_bPlaying == FALSE) {
	//获取视频服务器的GUID
	rv = Station_GetStationGUID(m_uiSession, &m_guidServer);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Station_GetStationGUID: failure.\n"));
		return  FALSE;
	}
	//获取资源描述
	rv = Resource_GetGUIDDescription(m_uiSession, m_guidServer, &m_guidDesc);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Resource_GetGUIDDescription: failure.\n"));
		return  FALSE;
	}
	
	// 获得服务器下的资源
	NcGUIDArray gaRsc;
	rv = Station_GetChildrenGUIDArray(m_uiSession, m_guidServer, &gaRsc);
	if (rv != NRCAP_SUCCESS) {
		slog(_T("Station_GetChildrenGUIDArray: failure.\n"));
		return  FALSE;
	}
	BYTE *pGUID = new NcGUID;
	memcpy(pGUID, m_guidServer, sizeof(NcGUID));
	BOOL bFound = FALSE;
	for (int i = 0; i < (int)gaRsc.uiNum; i++) {
		//通过此方法可以得到某个GUID对应的资源的信息。
		rv = Resource_GetGUIDDescription(m_uiSession, gaRsc.ga[i], &m_guidDesc);
		if (m_guidDesc.rscType == rsc_input_video) {
				memcpy(m_guidPM,m_guidDesc.guid, sizeof(NcGUID));
				bFound = TRUE;
				break;
			}
		}
		if (!bFound) {
			return FALSE;
		}
		NcGUID *guid = (NcGUID*)m_guidPM;
		rv = Resource_GetGUIDDescription(m_uiSession, *guid, &m_guidDesc);
		if (rv != NRCAP_SUCCESS) {
			slog(_T("Resource_GetGUIDDescription: failure.\n"));
			return  FALSE;
		}
		memcpy(m_guidCurInV, guid, sizeof(NcGUID));
		// 启动视频数据截取
		rv = NcStartStreamCapture(m_uiSession, m_guidCurInV, m_st, m_tt);
		if (rv != NRCAP_SUCCESS) {
			slog(_T("NcStartStreamCapture: failure.\n"));
		}
		//请求发送I帧
		rv = InputVideo_StartKeyFrame(m_uiSession, m_guidCurInV, m_st);
		if (rv != NRCAP_SUCCESS) {
			slog(_T("InputVideo_StartKeyFrame: failure.\n"));
		}
		delete []pGUID;
		// 设置播放窗口 设置播放位置
		rv = VADR_SetPlayWnd(m_hVARender, hWnd, TRUE);
		if (rv != NRCAP_SUCCESS) {
			slog(_T("VADR_SetPlayWnd: failure.\n"));
		}
		// 开始解码显示
		rv = VADR_StartPreview(m_hVARender);
		if (rv != NRCAP_SUCCESS) {
			slog(_T("VADR_StartPreview.\n"));
		}
		m_bPlaying = TRUE;
		Sleep(100);
		AfxBeginThread(Run, this);
		AfxBeginThread(NetDetectRecover,this);
		AfxBeginThread(LocalNetDetect,this); 
		return TRUE;
	}
	return TRUE;
}

//退出登录
void CNrcServer::Logout() {
	if (m_uiSession != NRCAP_INVALID_SESSION) {
		NcStopStreamCapture(m_uiSession, m_guidCurInV, m_st, m_tt);	
		m_uiSession = NRCAP_INVALID_SESSION;
		m_bPlaying = FALSE;
		m_bInitlized = FALSE;
		NcClose(m_uiSession);
		NcTerminate();
	}
	if (m_hVARender != NULL) {
		VADR_StopPreview(m_hVARender);
		VADR_Close(m_hVARender);
		m_hVARender = NULL;
	}
}

// 停止播放
BOOL CNrcServer::StopPlay(LPVOID pChannel) {
	//停止数据截取
	int rv1 = NcStopStreamCapture(m_uiSession, m_guidCurInV, m_st, m_tt);
	//停止解码显示
	int rv2 = VADR_StopPreview(m_hVARender);
	m_bPlaying = FALSE;
	if (rv1 == NRCAP_SUCCESS && rv2 == NRCAP_SUCCESS) {
		return TRUE ;
	}
	return FALSE;
}

BOOL CNrcServer::GetNetworkState() {
	return m_bNetWorkstate;
}

LPCTSTR CNrcServer::GetServerName() {
	return m_szServerName;
}

BOOL CNrcServer::GetServerState() {
	return m_bServerState;
}
