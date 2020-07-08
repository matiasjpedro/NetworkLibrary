#pragma once
#include <ostream>

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
