#include <stdarg.h>
#include <stdio.h>
#include <cfe/compiler.h>
#include <cfe/log.h>

static void cfe_log_message(const char *prefix, const char *fmt, va_list va)
{
	fprintf(stderr, "%s", prefix);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
}

void cfe_log_emergency(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[EMERG]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_alert(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ALERT]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_critical(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ CRIT]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ERROR]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_warning(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ WARN]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_notice(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ NOTE]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_info(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[ INFO]: ", fmt, ap);
	va_end(ap);
}

void cfe_log_debug(_unused_ int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	cfe_log_message("[DEBUG]: ", fmt, ap);
	va_end(ap);
}
