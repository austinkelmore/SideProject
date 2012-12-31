
#pragma once

#define UNUSED_VAR(x) (void)x

namespace Ecks
{
	namespace Assert
	{
		enum AssertBehavior
		{
			ASSERT_HALT,
			ASSERT_CONTINUE,
		};

		typedef AssertBehavior (*AssertHandler)(const char* condition,
												const char* message,
												const char* file,
												int line);

		AssertBehavior ReportFailure(const char* condition,
									 const char* file,
									 int line,
									 const char* msg,
									 ...);
	} // namespace Assert
} // namespace Ecks

#define MULTI_LINE_MACRO_BEGIN \
	do \
	{
#define MULTI_LINE_MACRO_END \
		__pragma(warning(push)) \
		__pragma(warning(disable:4127)) \
	} while(0) \
	__pragma(warning(pop))

#define ASSERTS_ENABLED

#define HALT() __debugbreak()

#ifdef ASSERTS_ENABLED
	#define DBG_ASSERT(xCondition) \
		MULTI_LINE_MACRO_BEGIN \
			if (!(xCondition)) \
			{ \
				if (Ecks::Assert::ReportFailure(#xCondition, __FILE__, __LINE__, 0) == Ecks::Assert::ASSERT_HALT) \
					HALT(); \
			} \
		MULTI_LINE_MACRO_END

	#define DBG_ASSERT_MSG(xCondition, xMessage, ...) \
		MULTI_LINE_MACRO_BEGIN \
			if (!(xCondition)) \
			{ \
				if (Ecks::Assert::ReportFailure(#xCondition, __FILE__, __LINE__, (xMessage), __VA_ARGS__) == Ecks::Assert::ASSERT_HALT) \
					HALT(); \
			} \
		MULTI_LINE_MACRO_END

	#define DBG_ASSERT_FAIL(xMessage, ...) \
		MULTI_LINE_MACRO_BEGIN \
			if (Ecks::Assert::ReportFailure(0, __FILE__, __LINE__, (xMessage), __VA_ARGS__) == Ecks::Assert::ASSERT_HALT) \
				HALT(); \
		MULTI_LINE_MACRO_END
#else
	#define DBG_ASSERT(xCondition) \
		do { UNUSED_VAR(xCondition); } while(0)
	#define DBG_ASSERT_MSG(xCondition, xMessage, ...) \
		do { UNUSED_VAR(xCondition); UNUSED_VAR(xMessage); } while(0)
	#define DBG_ASSERT_FAIL(xMessage, ...) \
		do { UNUSED_VAR(xMessage); } while(0)
#endif // ASSERTS_ENABLED
