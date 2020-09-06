#pragma once

enum PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_Disconnect,
	PT_MAX
};

enum ReplicationAction
{
	RA_Create,
	RA_Update,
	RA_Destroy,
	RA_MAX
};