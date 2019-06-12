#include "Packet.h"

Packet::Packet()
{
	this->data = std::map<int, cv::Mat>();
}

Packet::Packet(Packet* packet)
{
	this->data = std::map<int, cv::Mat>(packet->getData());
}

std::map<int, cv::Mat> Packet::getData()
{
	return data;
}

void Packet::addData(int index, cv::Mat payload)
{
	data[index] = payload;
}

bool Packet::hasData()
{
	return data.empty();
}