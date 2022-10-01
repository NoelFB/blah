#include <blah_common.h>
#include <blah_app.h>

using namespace Blah;

void Log::info(const char* format, ...)
{
	char msg[max_length];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * max_length, format, ap);
	va_end(ap);

	if (App::is_running() && App::config().on_log)
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
	char msg[max_length];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * max_length, format, ap);
	va_end(ap);

	if (App::is_running() && App::config().on_log)
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
	char msg[max_length];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * max_length, format, ap);
	va_end(ap);

	if (App::is_running() && App::config().on_log)
	{
		App::config().on_log(msg, Category::Error);
	}
	else
	{
		printf("ERROR: %s\n", msg);
	}
}