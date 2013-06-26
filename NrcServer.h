#pragma once
#include "windows.h"
#include "afxwin.h"
#include "VAStorage.h"
#include "VARender.h"
#include "NrcapEvent.h"
#include "NrcapError.h"
#include "NrcappcSDK.h"
#include "CServerStatusReport.h"

class NrcServer : public CServerStatusReport{
public:
	NrcServer();
	~NrcServer();
	//登录视频服务器
	BOOL Login(LPCTSTR lpszAddress, int nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword);
	//设置时间通知回调函数
	void SetEventCallback(void (*EventCallback)(int nEvent));
	//播放视频
	BOOL StartPlay(LPVOID pChannel, HWND hWnd);
	//停止播放视频
	BOOL StopPlay(LPVOID pChannel);
	//退出视频服务器
	void Logout();
	//处理过滤事件函数
	static UINT		Run(LPVOID pParam);
	//检测视频服务器的网络状态
	static UINT		NetDetectRecover(LPVOID pParam);
	//检测本地网络的连接状态
	static UINT		LocalNetDetect(LPVOID pParam);
	//获取服务器的名称
	LPCTSTR			GetServerName();
	//启动测试服务器网络状态的线程
	void			StartThread();
	//获取网络的状态
	BOOL			GetNetworkState();
	//获取服务器连接状态
	BOOL			GetServerState();
public:
	HANDLE			m_hVARender;
	HANDLE			m_hVAStorage;
	HWND			m_hWnd;
	NcGUID			m_guidServer;
	int				m_nErrorCode;
	BOOL			m_bInitlized;
	DWORD			m_lastPlayingTime;
	BOOL			m_bThread;
	BOOL			m_bNetWorkstate;
	BOOL			m_bserverState;
	BOOL			m_bNetworkFlag;
	DWORD			m_dwLastTime;
private:
	CStatic			m_stcPlayWnd;
	UINT			m_uiSession;
	BOOL			m_bPlaying;
	NcGUID			m_guidPM;
	NcGUID			m_guidCurInV;		//正在播放的视频GUID
	NcGUID			m_guidCurInA;		//当前播放的视频绑定的音频的GUID
	StreamType		m_st;				//流类型
	TransferType	m_tt;				//流传输类型
	LPVOID			pChannel;
	UINT			m_uiSesson;
	CString			ServerName;
};

