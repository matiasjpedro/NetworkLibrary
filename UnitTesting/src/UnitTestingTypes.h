#pragma once
#include <string>
#include <ostream>
#include <iostream>

#define MAKE_ENUM(VAR) VAR,
#define MAKE_STRINGS(VAR) #VAR,
#define MAKE_LOGGABLE_ENUM(source, enumName) \
    enum class enumName : int { \
	source(MAKE_ENUM) \
	LENGHT \
    };\
const char* const enumName##String[] = { \
    source(MAKE_STRINGS) \
	#enumName \
    };\
static std::ostream& operator<<(std::ostream& os, enumName enumValue) {\
	return os << enumName##String[(int)enumValue]; \
}

template<typename T>
class ITesteable
{
public:

	void RunTests()
	{
		MockDataForTests();

		std::string OutErrMsg;

		//Print the name of the enum which is going to be tested:
		std::cout << "[" << (T)((int)T::LENGHT)  << "]" << std::endl;

		float succeedTest = 0;

		for (int i = 0; i < (int)T::LENGHT; i++)
		{
			std::cout << std::endl << "Start test: " << (T)i << std::endl;

			if (RunTest((T)i, OutErrMsg))
			{
				succeedTest++;
				std::cout << "Test results: [SUCCEED]" << std::endl;
			}
			else
			{
				std::cout << "Test results: [FAIL] -> ErrMsg: " << OutErrMsg << std::endl; 
			}
		}

		float testPercentage = (succeedTest / (float)T::LENGHT) * 100;
		std::cout << std::endl << testPercentage <<  "% , press key to continue..." << std::endl;
		
		std::getchar();
	}

protected:

	virtual void MockDataForTests() {};
	virtual bool RunTest(T TestToRun, std::string& OutErrMsg) = 0;
};