#include <vector>
#include "iostream"
#include "CServerStatusReport.h"

using namespace std;
typedef void(*EventCallback)(int nEevent, LPCTSTR ServerName);
class CVIDetect {
public:
	CVIDetect();
	//注册需要检测的服务器
	void Register(CServerStatusReport* pServer);
	//对注册的服务进行检测
	void StartDetect();
	//设置事件处理回调函数
	void SetEventCallback(EventCallback fnCallback);
public:
	vector<CServerStatusReport*> m_vectorServers;
	EventCallback m_fnEventCallback;
};