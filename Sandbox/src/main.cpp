#include "UnitTesting/NetworkLibraryTests.h"
#include "Chat.h"
#include "NetDriver.h"

int main()
{
	NetworkLibraryTests::RunInternalTests();

 	NetDriver CurrentNetDriver;
 	CurrentNetDriver.Init();
 	CurrentNetDriver.Tick();
}