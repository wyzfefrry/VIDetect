#ifndef _SERVERSR_H
#define	_SERVERSR_H

class CServerStatusReport {
public:
	//��ȡ����״̬
	virtual BOOL	GetNetworkState()	= 0;
	//��ȡ����������
	virtual LPCTSTR GetServerName()		= 0;
	//��ȡ������״̬
	virtual BOOL	GetServerState()	= 0;
	~CServerStatusReport() {};
};
#endif _SERVERSR_H