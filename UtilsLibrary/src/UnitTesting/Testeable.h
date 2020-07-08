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
		std::cout << "[" << (T)((int)T::LENGHT) << "]" << std::endl;

		float succeedTest = 0;

		for (int i = 0; i < (int)T::LENGHT; i++)
		{
			std::cout << std::endl << "Start test: " << (T)i << std::endl;

			if (RunTest((T)i))
			{
				succeedTest++;
				std::cout << "Test results: [SUCCEED]" << std::endl;
			}
			else
			{
				std::cout << "Test results: [FAIL] -> ErrMsg: " << LastErrorStr << std::endl;
			}
		}

		float testPercentage = (succeedTest / (float)T::LENGHT) * 100;
		std::cout << std::endl << testPercentage << "% , press key to continue..." << std::endl;

		std::getchar();
	}

protected:

	virtual void MockDataForTests() {};
	virtual bool RunTest(T TestToRun) = 0;
};