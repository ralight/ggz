#include "net.h"
#include <iostream>

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

