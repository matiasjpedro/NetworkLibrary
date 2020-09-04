#include "UnitTesting/NetworkLibraryTests.h"
#include "Chat.h"
#include "NetDriver.h"

int main()
{
	NetworkLibraryTests::RunInternalTests();

	//DoTCPChat();

	NetDriver CurrentNetDriver;

	CurrentNetDriver.Init();

	CurrentNetDriver.Tick();
}