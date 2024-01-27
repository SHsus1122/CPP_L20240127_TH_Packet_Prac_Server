#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

enum class EPacketType
{
	Caculate = 10,
	Image = 20,
	MAX
};

#pragma pack(push, 1)
typedef struct _Header
{
	unsigned short Length;
	unsigned short PacketType;
} Header;

typedef struct _Data
{
	int FirstNumber;
	int SecondNumber;
	char Operator;
} Data;	// Packet

#pragma pack(pop)

#endif // __PACKET_H__
