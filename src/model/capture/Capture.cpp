#include "Capture.h"

Capture::Capture()
{
	this->packets = std::list<Packet*>();
}

void Capture::addPacket(Packet* packet)
{
	packets.push_back(packet);
}

std::list<Packet*> Capture::getPackets()
{
	return packets;
}