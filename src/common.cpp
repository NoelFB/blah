#include <blah/common.h>
#include <blah/app.h>
#include <stdarg.h> // for logging methods
#include <stdio.h>  // for sprintf

using namespace Blah;

void Log::info(const char* format, ...)
{
	char msg[BLAH_MESSAGE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * BLAH_MESSAGE, format, ap);
	va_end(ap);

	if (App::config().on_log)
	{
		App::config().on_log(msg, Category::Info);
	}
	else
	{
		printf("%s\n", msg);
	}
}

void Log::warn(const char* format, ...)
{
	char msg[BLAH_MESSAGE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * BLAH_MESSAGE, format, ap);
	va_end(ap);

	if (App::config().on_log)
	{
		App::config().on_log(msg, Category::Warning);
	}
	else
	{
		printf("WARN: %s\n", msg);
	}
}

void Log::error(const char* format, ...)
{
	char msg[BLAH_MESSAGE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * BLAH_MESSAGE, format, ap);
	va_end(ap);

	if (App::config().on_log)
	{
		App::config().on_log(msg, Category::Error);
	}
	else
	{
		printf("ERROR: %s\n", msg);
	}
}