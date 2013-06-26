#ifndef _SERVERSPI_H
#define	_SERVERSPI_H

class CServerStatusReport {
public:
	virtual BOOL	GetNetworkState() = 0;
	virtual LPCTSTR GetServerName() = 0;
	virtual BOOL	GetServerState() = 0;
	~CServerStatusReport() {};
};
#endif _SERVERSPI_H