#include "stdafx.h"
#include "VIDetect.h"

void fnCallBack(int nEvent, CString serverName) {
	CString szInfo;
	switch(nEvent) {
		case 0x0:
			AfxMessageBox(_T("���쳣"));
			break;
		case 0x1:
			szInfo.Format(_T("%s :�����ж�"),serverName);
			AfxMessageBox(szInfo);
			break;
		case 0x2:
			szInfo.Format(_T("%s :�������Ͽ�����"), serverName);
			AfxMessageBox(szInfo);
			break;
		case 0x3:
			szInfo.Format(_T("%s : ����ָ�"), serverName);
			AfxMessageBox(szInfo);
			break;
		case  0x4:
			szInfo.Format(_T("%s :�������ָ�����"),serverName);
			AfxMessageBox(szInfo);
			break;
		default:
			AfxMessageBox(_T("δ֪����"));
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
				//�����ж�
				fnCallBack(0x1, CString((*m_listServers_Iterator)->GetServerName()));
				bNetworkFlag = TRUE;
			} else
 			if ((*m_listServers_Iterator)->GetNetworkState()== TRUE && bNetworkFlag == TRUE) {
				//����ָ�
				fnCallBack(0x3, CString((*m_listServers_Iterator)->GetServerName()));
				bNetworkFlag = FALSE;
			}
			if((*m_listServers_Iterator)->GetServerState() == FALSE && bServerFlag == FALSE) {
				//�������Ͽ�
				fnCallBack(0x2,CString((*m_listServers_Iterator)->GetServerName()));
				bServerFlag = TRUE;
			}
			if((*m_listServers_Iterator)->GetServerState() == TRUE && bServerFlag == TRUE) {
				//�������ָ�
				fnCallBack(0x4,(*m_listServers_Iterator)->GetServerName());
				bServerFlag = FALSE;
			}
			
		} 
	}
}


