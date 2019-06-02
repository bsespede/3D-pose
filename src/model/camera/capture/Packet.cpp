#include "Packet.h"

Packet::Packet()
{
	this->data = map<int, Mat>();
}

Packet::Packet(Packet* packet)
{
	this->data = map<int, Mat>(packet->getData());
}

map<int, Mat> Packet::getData()
{
	return data;
}

void Packet::addData(int index, Mat payload)
{
	data[index] = payload;
}

bool Packet::hasData()
{
	return data.empty();
}