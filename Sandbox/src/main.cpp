#include "UnitTesting/NetworkLibraryTests.h"
#include "TrafficTester.h"

int main()
{
	//NetworkLibraryTests::RunInternalTests();

 	TrafficTester traffic_Tester;
    traffic_Tester.init();
    traffic_Tester.update();
}