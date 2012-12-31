
#include "BasicMacros.h"
#include <cstdio>
#include <cstdarg>
#include "Logging.h"

namespace Ecks
{
	namespace Assert
	{
		AssertBehavior DefaultAssertHandler(const char* condition, const char* msg, const char* file, const int line)
		{
			Logging::Log(LOG_Assert, "%s(%d): Assert Failure: ", file, line);

			if (condition != NULL)
				Logging::Log(LOG_Assert, "'%s' ", condition);

			if (msg != NULL)
				Logging::Log(LOG_Assert, "%s", msg);

			Logging::Log(LOG_Assert, "\n");

			return ASSERT_HALT;
		}

		AssertBehavior ReportFailure(const char* condition, const char* file, int line, const char* msg, ...)
		{
			const char* message = NULL;
			if (msg != NULL)
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
} // namespace Ecks