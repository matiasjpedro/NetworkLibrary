#pragma once
#include "UnitTestingTypes.h"

#pragma region ITesteable
#define CLASS_A_TESTS_ENUM(DO) \
    DO(TEST_DMG) \
    DO(TEST_HEALTH) \
    DO(TEST_MOVEMENT) \
	DO(TEST_SCORE)

MAKE_LOGGABLE_ENUM(CLASS_A_TESTS_ENUM, ETestExampleTests)
#pragma endregion

class TestExample : public ITesteable<ETestExampleTests>
{
private:

	int Damage;
	int Health;
	int Movement;
	int Score;

protected:

	void MockDataForTests() override;
	bool RunTest(ETestExampleTests TestToRun, std::string& OutErrMsg) override;

};