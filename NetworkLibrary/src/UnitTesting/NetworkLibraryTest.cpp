#include "NetworkLibraryTests.h"
#include "SerializationTests.h"

void NetworkLibraryTests::RunInternalTests()
{
	SerializationTests SerTest;
	SerTest.RunTests();
}