#include "stdafx.h"
#include "VIDetect.h"

void fnCallBack(int nEvent, CString serverName) {
	CString szInfo;
	switch(nEvent) {
		case 0x0:
			AfxMessageBox(_T("无异常"));
			break;
		case 0x1:
			szInfo.Format(_T("%s :网络中断"),serverName);
			AfxMessageBox(szInfo);
			break;
		case 0x2:
			szInfo.Format(_T("%s :服务器断开连接"), serverName);
			AfxMessageBox(szInfo);
			break;
		case 0x3:
			szInfo.Format(_T("%s : 网络恢复"), serverName);
			AfxMessageBox(szInfo);
			break;
		case  0x4:
			szInfo.Format(_T("%s :服务器恢复连接"),serverName);
			AfxMessageBox(szInfo);
			break;
		default:
			AfxMessageBox(_T("未知错误"));
	}
}


void VIDetect::Register(CServerStatusReport* context) { 
	m_listServers.push_back(context);
}

void VIDetect::StartDetect() {
	BOOL bNetworkFlag = FALSE;
	BOOL bServerFlag = FALSE;
	list<CServerStatusReport*>::iterator m_listServers_Iterator;
	while(1) {
		for (m_listServers_Iterator = m_listServers.begin(); m_listServers_Iterator != m_listServers.end(); m_listServers_Iterator++) {
			Sleep(2000);
			if((*m_listServers_Iterator)->GetNetworkState() == FALSE && bNetworkFlag == FALSE) {
				//网络中断
				fnCallBack(0x1, CString((*m_listServers_Iterator)->GetServerName()));
				bNetworkFlag = TRUE;
			} else
 			if ((*m_listServers_Iterator)->GetNetworkState()== TRUE && bNetworkFlag == TRUE) {
				//网络恢复
				fnCallBack(0x3, CString((*m_listServers_Iterator)->GetServerName()));
				bNetworkFlag = FALSE;
			}
			if((*m_listServers_Iterator)->GetServerState() == FALSE && bServerFlag == FALSE) {
				//服务器断开
				fnCallBack(0x2,CString((*m_listServers_Iterator)->GetServerName()));
				bServerFlag = TRUE;
			}
			if((*m_listServers_Iterator)->GetServerState() == TRUE && bServerFlag == TRUE) {
				//服务器恢复
				fnCallBack(0x4,(*m_listServers_Iterator)->GetServerName());
				bServerFlag = FALSE;
			}
			
		} 
	}
}


