#pragma once
#include "SerializationTests.h"

class NetworkLibraryTests 
{
	void RunInternalTests()
	{
		ESerializationTests SerTests;
		SerTests.RunTests();
	}
};