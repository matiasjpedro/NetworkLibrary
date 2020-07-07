#include "TestExample.h"

void TestExample::MockDataForTests()
{
	Damage = 10;
	Health = 100;
	Movement = 20;
	Score = 500;
}

bool TestExample::RunTest(ETestExampleTests TestToRun)
{
	switch (TestToRun)
	{
		case ETestExampleTests::TEST_DMG:
		{
			return Damage == 10;
		}
		case ETestExampleTests::TEST_HEALTH:
		{
			return Health != 0;
		}
		case ETestExampleTests::TEST_MOVEMENT:
		{
			const int MaxSpeed = 10;
			const bool bExceedMaxSpeed = Movement > MaxSpeed;

			if (bExceedMaxSpeed)
			{
				LastErrorStr.append("Exceed Max Speed!");
				return false;
			}

			return true;
		}	
		case ETestExampleTests::TEST_SCORE:
		{
			return Score == 500;
		}

		default:
			break;
	}

	return true;
}

