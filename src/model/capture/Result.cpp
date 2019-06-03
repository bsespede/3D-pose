#include "Capture.h"

Capture::Capture()
{
	this->packets = list<Packet*>();
}

void Capture::addPacket(Packet* packet)
{
	packets.push_back(packet);
}

list<Packet*> Capture::getPackets()
{
	return packets;
}