#pragma once
#include "windows.h"
#include "afxwin.h"
#include "VAStorage.h"
#include "VARender.h"
#include "NrcapEvent.h"
#include "NrcapError.h"
#include "NrcappcSDK.h"
#include "CServerStatusReport.h"
//typedef void (*fnEventCallback)(int nEvent);

class NrcServer : public CServerStatusReport{

public:
	NrcServer();
	~NrcServer();
	//��¼��Ƶ������
	BOOL Login(LPCTSTR lpszAddress, int nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword);
	//����ʱ��֪ͨ�ص�����
	void SetEventCallback(void (*EventCallback)(int nEvent));
	//������Ƶ
	BOOL StartPlay(LPVOID pChannel, HWND hWnd);
	//ֹͣ������Ƶ
	BOOL StopPlay(LPVOID pChannel);
	//�˳���Ƶ������
	void Logout();
	//��������¼�����
	static UINT		Run(LPVOID pParam);
	//�����Ƶ������������״̬
	static UINT		NetDetectRecover(LPVOID pParam);
	//��Ȿ�����������״̬
	static UINT		LocalNetDetect(LPVOID pParam);
	//��ȡ������������
	LPCTSTR			GetServerName();
	//�������Է���������״̬���߳�
	void			StartThread();
	//��ȡ�����״̬
	BOOL			GetNetworkState();
	//��ȡ����������״̬
	BOOL			GetServerState();
public:
	HANDLE			m_hVARender;
	HANDLE			m_hVAStorage;
	HWND			m_hWnd;
	NcGUID			m_guidServer;
private:
	CStatic			m_stcPlayWnd;
	LPCTSTR			m_lpszAddress;
	LPCTSTR			m_lpszUserName;
	LPCTSTR			m_lpszPassword;
	int				m_nPort;
	UINT			m_uiSession;
	BOOL			m_bPlaying;
	NcGUID			m_guidPM;
	NcGUID			m_guidCurInV;		//���ڲ��ŵ���ƵGUID
	NcGUID			m_guidCurInA;		//��ǰ���ŵ���Ƶ�󶨵���Ƶ��GUID
	StreamType		m_st;				//������
	TransferType	m_tt;				//����������
	LPVOID			pChannel;
	UINT			m_uiSesson;
	CString			ServerName;
public:
	UINT			g_bEvent;
	UINT			g_uiEventID;
	int				g_nErrorCode;
	BOOL			g_bInitlized;
	DWORD			g_lastPlayingTime;
	BOOL			g_bThread;
	BOOL			g_bNetWorkstation;
	BOOL			g_bServerStaion;
	BOOL			g_bNetworkFlag;
	DWORD			g_dwLastTime;
};

