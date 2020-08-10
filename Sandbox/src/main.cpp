#include "UnitTesting/NetworkLibraryTests.h"
#include "Chat.h"

int main()
{
	NetworkLibraryTests::RunInternalTests();

	DoTCPChat();
}