#include "stdafx.h"
#include "CVIDetect.h"

void fnEventCallBack(int nEvent, LPCTSTR m_szServerName) {
	switch(nEvent) {
		case 0x0:
			TRACE(_T("无异常\n"));
			break;
		case 0x1:
			TRACE(_T("%s :网络断开\n"), m_szServerName);
			break;
		case 0x2:
			TRACE(_T("%s :服务器断开\n"), m_szServerName);
			break;
		case 0x3:
			TRACE(_T("%s : 网络恢复\n"), m_szServerName);
			break;
		case 0x4:
			TRACE(_T("%s :服务器恢复\n"), m_szServerName);
			break;
		default:
			TRACE(_T("未知错误\n"));
	}
}
CVIDetect::CVIDetect() {
	SetEventCallback(fnEventCallBack);
}

void CVIDetect::SetEventCallback(EventCallback fnCallBack) {
	m_fnEventCallback = fnCallBack;
}
void CVIDetect::Register(CServerStatusReport* pServer) { 
	m_vectorServers.push_back(pServer);
}

void CVIDetect::StartDetect() {
	BOOL bNetworkFlag	= FALSE;
	BOOL bServerFlag	= FALSE;
	vector<CServerStatusReport*>::iterator vectorServers_Iterator;
	while(1) {
			for (vectorServers_Iterator = m_vectorServers.begin();
				 vectorServers_Iterator != m_vectorServers.end();
				 vectorServers_Iterator++) {
				Sleep(2000);
				if((*vectorServers_Iterator)->GetNetworkState() == FALSE && bNetworkFlag == FALSE) {
					//网络中断
					m_fnEventCallback(0x1, ((*vectorServers_Iterator)->GetServerName()));
					bNetworkFlag = TRUE;
				} else
 				if ((*vectorServers_Iterator)->GetNetworkState()== TRUE && bNetworkFlag == TRUE) {
					//网络恢复
					m_fnEventCallback(0x3, ((*vectorServers_Iterator)->GetServerName()));
					bNetworkFlag = FALSE;
				}
				if((*vectorServers_Iterator)->GetServerState() == FALSE && bServerFlag == FALSE) {
					//服务器断开
					m_fnEventCallback(0x2,((*vectorServers_Iterator)->GetServerName()));
					bServerFlag = TRUE;
				}
				if((*vectorServers_Iterator)->GetServerState() == TRUE && bServerFlag == TRUE) {
					//服务器恢复
					m_fnEventCallback(0x4,(*vectorServers_Iterator)->GetServerName());
					bServerFlag = FALSE;
				}
			} 
	}
}


