#include <blah/core/log.h>
#include <blah/core/app.h>
#include <stdarg.h> // for logging methods
#include <stdio.h>  // for sprintf

using namespace Blah;

void Log::print(const char* format, ...)
{
	char msg[BLAH_MESSAGE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * BLAH_MESSAGE, format, ap);
	va_end(ap);

	if (App::is_running() && App::config()->on_info != nullptr)
	{
		App::config()->on_info(msg);
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

	if (App::is_running() && App::config()->on_warn != nullptr)
	{
		App::config()->on_warn(msg);
	}
	else
	{
		printf("\033[01;33mWARN:\033[0m\t%s\n", msg);
	}
}

void Log::error(const char* format, ...)
{
	char msg[BLAH_MESSAGE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * BLAH_MESSAGE, format, ap);
	va_end(ap);

	if (App::is_running() && App::config()->on_error != nullptr)
	{
		App::config()->on_error(msg);
	}
	else
	{
		printf("\033[1;31mERROR:\033[0m\t%s\n", msg);
	}
}