#pragma once

#include <list>
#include "model/camera/capture/Packet.h"

using namespace std;

class Capture
{
public:
	Capture();
	void addPacket(Packet* packet);
	std::list<Packet*> getPackets();
private:
	std::list<Packet*> packets;
};
