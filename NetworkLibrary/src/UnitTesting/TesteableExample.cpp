#include "TesteableExample.h"

void TesteableExample::MockDataForTests()
{
	Damage = 10;
	Health = 100;
	Movement = 20;
	Score = 500;
}

bool TesteableExample::RunTest(ETesteableExampleTests TestToRun)
{
	switch (TestToRun)
	{
		case ETesteableExampleTests::TEST_DMG:
		{
			return Damage == 10;
		}
		case ETesteableExampleTests::TEST_HEALTH:
		{
			return Health != 0;
		}
		case ETesteableExampleTests::TEST_MOVEMENT:
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
		case ETesteableExampleTests::TEST_SCORE:
		{
			return Score == 500;
		}

		default:
			break;
	}

	return true;
}

