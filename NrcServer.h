#pragma once
#include "windows.h"
#include "afxwin.h"
#include "VAStorage.h"
#include "VARender.h"
#include "NrcapEvent.h"
#include "NrcapError.h"
#include "NrcappcSDK.h"
#include "CServerStatusReport.h"

#define NRCAP_INVALID_SESSION 0xFFFFFFFF

class CNrcServer : public CServerStatusReport{
public:
	CNrcServer();
	~CNrcServer();
	//��¼��Ƶ������
	BOOL Login(LPCTSTR lpszAddress, int nPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword);
	//����ʱ��֪ͨ�ص�����
	void SetEventCallback(void (*EventCallback)(int nEvent));
	//������Ƶ
	BOOL StartPlay(LPVOID m_pChannel, HWND hWnd);
	//ֹͣ������Ƶ
	BOOL StopPlay(LPVOID m_pChannel);
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
	BOOL			m_bPlaying;
	NcGUID			m_guidPM;
	NcGUID			m_guidCurInV;		//���ڲ��ŵ���ƵGUID
	NcGUID			m_guidCurInA;		//��ǰ���ŵ���Ƶ�󶨵���Ƶ��GUID
	StreamType		m_st;				//������
	TransferType	m_tt;				//����������
	LPVOID			m_pChannel;
	UINT			m_uiSesson;
	CString			m_ServerName;
public:
	UINT				m_uiSession;
	int					m_nServerGuid;
	int					m_nServerDesc;	
	int					m_nServerRsc;
	NcGUIDDescription	m_guidDesc;
	int					m_nStreamCapture;
	int					m_nStartKeyFrame;
};

