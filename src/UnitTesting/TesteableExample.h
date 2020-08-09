#pragma once
#include "Testeable.h"

#pragma region ITesteable
#define CLASS_A_TESTS_ENUM(DO) \
    DO(TEST_DMG) \
    DO(TEST_HEALTH) \
    DO(TEST_MOVEMENT) \
	DO(TEST_SCORE)

MAKE_LOGGABLE_ENUM(CLASS_A_TESTS_ENUM, ETesteableExampleTests)
#pragma endregion

class TesteableExample : public ITesteable<ETesteableExampleTests>
{
private:

	int Damage;
	int Health;
	int Movement;
	int Score;

protected:

	void MockDataForTests() override;
	bool RunTest(ETesteableExampleTests TestToRun) override;

};