//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <stdio.h>

#define LOG_DEBUG(fmt, ...)	LOG_INTERNAL("DEBUG", fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)	LOG_INTERNAL("INFO", fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)	LOG_INTERNAL("WARN", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)	LOG_INTERNAL("ERROR", fmt, ##__VA_ARGS__)

#define FLOG_DEBUG(fmt, ...) FLOG_INTERNAL("DEBUG", fmt, ##__VA_ARGS__)
#define FLOG_INFO(fmt, ...)	 FLOG_INTERNAL("INFO", fmt, ##__VA_ARGS__)
#define FLOG_WARN(fmt, ...)	 FLOG_INTERNAL("WARN", fmt, ##__VA_ARGS__)
#define FLOG_ERROR(fmt, ...) FLOG_INTERNAL("ERROR", fmt, ##__VA_ARGS__)


#define FLOG_INTERNAL(level, fmt, ...)	                                                		\
	do {                                                                                    	\
        fprintf(                                                                            	\
			stderr,                                                                         	\
			"[" level "] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__    \
		); 																						\
	} while (0)

#if defined(DEBUG) && DEBUG > 0
#define LOG_INTERNAL(level, fmt, ...)	                                                		\
	do { FLOG_INTERNAL(level, fmt, ##__VA_ARGS__); } while (0)
#else
#define LOG_INTERNAL(level, fmt, ...) do {} while (0) /* Don't print in release */
#endif
