#include <vector>
#include "iostream"
#include "CServerStatusReport.h"

using namespace std;
typedef void(*EventCallback)(int nEevent, LPCTSTR ServerName);
class CVIDetect {
public:
	CVIDetect();
	//ע����Ҫ���ķ�����
	void Register(CServerStatusReport* pServer);
	//��ע��ķ�����м��
	void StartDetect();
	//�����¼�����ص�����
	void SetEventCallback(EventCallback fnCallback);
public:
	vector<CServerStatusReport*> m_vectorServers;
	EventCallback m_fnEventCallback;
};