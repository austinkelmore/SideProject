
#include "BasicMacros.h"
#include <cstdio>
#include <cstdarg>
#include "Logging.h"

namespace TC
{
	namespace Assert
	{
		AssertBehavior DefaultAssertHandler(const char* condition, const char* msg, const char* file, const int line)
		{
			Logging::Log(LOG_Assert, "%s(%d): Assert Failure: '%s' %s", file, line, condition ? condition : "", msg ? msg : "");

			return ASSERT_HALT;
		}

		AssertBehavior ReportFailure(const char* condition, const char* file, int line, const char* msg, ...)
		{
			const char* message = nullptr;
			if (msg != nullptr)
			{
				char messageBuffer[1024];
				{
					va_list args;
					va_start(args, msg);
					vsnprintf_s(messageBuffer, 1024, msg, args);
					va_end(args);
				}

				message = messageBuffer;
			}

			return DefaultAssertHandler(condition, message, file, line);
		}

	} // namespace Assert
} // namespace TC