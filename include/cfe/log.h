#ifndef LIBCFE_CFE_LOG_H
#define LIBCFE_CFE_LOG_H


extern void cfe_log_emergency(const char *fmt, ...);
extern void cfe_log_alert(const char *fmt, ...);
extern void cfe_log_critical(const char *fmt, ...);
extern void cfe_log_error(const char *fmt, ...);
extern void cfe_log_warning(const char *fmt, ...);
extern void cfe_log_notice(const char *fmt, ...);
extern void cfe_log_info(const char *fmt, ...);
extern void cfe_log_debug(int level, const char *fmt, ...);


#endif
