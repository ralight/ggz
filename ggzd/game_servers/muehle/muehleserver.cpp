#include "muehleserver.h"
#include <iostream>

MuehleServer::MuehleServer ()
: GGZGameServer () {
}

MuehleServer::~MuehleServer () {
}

void MuehleServer::stateEvent () {
	std::cout << "Muehle: stateEvent" << std::endl;
}

void MuehleServer::joinEvent () {
	std::cout << "Muehle: joinEvent" << std::endl;
}

void MuehleServer::leaveEvent () {
	std::cout << "Muehle: leaveEvent" << std::endl;
}

void MuehleServer::dataEvent () {
	std::cout << "Muehle: dataEvent" << std::endl;
}

void MuehleServer::errorEvent () {
	std::cout << "Muehle: errorEvent" << std::endl;
}

