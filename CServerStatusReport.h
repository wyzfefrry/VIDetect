#ifndef _SERVERSR_H
#define	_SERVERSR_H

class CServerStatusReport {
public:
	//获取网络状态
	virtual BOOL	GetNetworkState()	= 0;
	//获取服务器名字
	virtual LPCTSTR GetServerName()		= 0;
	//获取服务器状态
	virtual BOOL	GetServerState()	= 0;
	~CServerStatusReport() {};
};
#endif _SERVERSR_H