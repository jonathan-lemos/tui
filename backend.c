/** @file backend/backend.c
 * Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#if   defined(_WIN32)
#include "backend/backend_win32.c"
#elif defined(__linux__)
#include "backend/backend_posix.c"
#else
#error "Only real computers are supported."
#endif
