#include <list>
#include "iostream"
using namespace std;
#include "CServerStatusReport.h"

class VIDetect {
public:
	~VIDetect();
	void Register(CServerStatusReport* context);
	void StartDetect();
public:
	list<CServerStatusReport*> m_listServers;
};