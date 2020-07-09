#pragma once
#include <string>
#include <iostream>
#include "UnitTestingTypes.h"

static std::string LastErrorStr;

template<typename T>
class ITesteable
{

public:

	void RunTests()
	{
		LastErrorStr.clear();

		MockDataForTests();

		//Print the name of the enum which is going to be tested:
		std::cout << "[" << (T)((int)T::LENGHT) << "]" << '\n';

		float succeedTest = 0;

		for (int i = 0; i < (int)T::LENGHT; i++)
		{
			std::cout << '\n' << "Start test: " << (T)i << '\n';

			if (RunTest((T)i))
			{
				succeedTest++;
				std::cout << "Test results: [SUCCEED]" << '\n';
			}
			else
			{
				std::cout << "Test results: [FAIL] -> ErrMsg: " << LastErrorStr << '\n';
			}
		}

		float testPercentage = (succeedTest / (float)T::LENGHT) * 100;
		std::cout << '\n' << testPercentage << "% , press key to continue..." << '\n';

		std::getchar();
	}

protected:

	virtual void MockDataForTests() {};
	virtual bool RunTest(T TestToRun) = 0;
};