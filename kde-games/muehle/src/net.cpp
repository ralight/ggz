#include "net.h"
#include <iostream>

using namespace std;

Net::Net()
{
}

Net::~Net()
{
}

void Net::output(QString s)
{
	cout << "[" << s.latin1() << "]" << endl;
}

