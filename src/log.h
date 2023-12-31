//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <stdio.h>

#define LOG_DEBUG(fmt, args...)	LOG_INTERNAL("DEBUG", fmt, args)
#define LOG_INFO(fmt, args...)	LOG_INTERNAL("INFO", fmt, args)
#define LOG_WARN(fmt, args...)	LOG_INTERNAL("WARN", fmt, args)
#define LOG_ERROR(fmt, args...)	LOG_INTERNAL("ERROR", fmt, args)

#if defined(DEBUG) && DEBUG > 0
#define LOG_INTERNAL(level, fmt, args...)	                                                \
	do {                                                                                    \
        fprintf(                                                                            \
			stderr,                                                                         \
			"[" level "] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##args       \
		); 																					\
	} while (0)
#else
#define LOG_INTERNAL(level, fmt, args...) do {} while (0) /* Don't print in release */
#endif
