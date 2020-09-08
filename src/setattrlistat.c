/*
 * atcalls.c
 *
 * Copyright (c) 2013-2015 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

 /*
  * NOTICE: This file was modified by Ken Cunningham in November 2018 to allow
  * for use as a supporting file for MacPorts legacy support library. This notice
  * is included in support of clause 2.2 (b) of the Apple Public License,
  * Version 2.0.
  */


/* MP support header */
#include "MacportsLegacySupport.h"
#if __MP_LEGACY_SUPPORT_SETATTRLISTAT__


#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ucred.h>
#include <sys/shm.h>
#include <sys/unistd.h>

#include <assert.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


/* this is some apple internal magic */
#include <sys/syscall.h>
#include <fcntl.h>

#define PROTECT_ERRNO(what)  ({ int __err = (errno); what; errno = __err; })
#define ERR_ON(code, what)   if (what) { errno = (code); return -1; }

#define _ATCALL(fd, p, onerr, what)                             \
    ({  typeof(what) __result;                                  \
        int oldCWD = -1;                                        \
        if (fd != AT_FDCWD && p[0] != '/') {                    \
            oldCWD = open(".", O_RDONLY);                       \
            if (__pthread_fchdir(-1) < 0 && oldCWD != -1) {     \
                close(oldCWD); oldCWD = -1;                     \
            }                                                   \
            if (__pthread_fchdir(fd) < 0) {                     \
                PROTECT_ERRNO(__pthread_fchdir(oldCWD));        \
                if (oldCWD != -1) PROTECT_ERRNO(close(oldCWD)); \
                return onerr;                                   \
            }                                                   \
        }                                                       \
        __result = (what);                                      \
        if (fd != AT_FDCWD && p[0] != '/') {                    \
            PROTECT_ERRNO(__pthread_fchdir(oldCWD));            \
            if (oldCWD != -1) PROTECT_ERRNO(close(oldCWD));     \
        }                                                       \
        __result;                                               \
    })

#define ATCALL(fd, p, what)  _ATCALL(fd, p, -1, what)

int setattrlistat(int dirfd, const char *pathname, void *a,
                  void *buf, size_t size, unsigned long flags)
{
#ifdef __LP64__
    /* This is fricken stupid */
    unsigned int _flags = (unsigned int)flags;
    assert ((unsigned long)_flags == flags);

    return ATCALL(dirfd, pathname, setattrlist(pathname, a, buf, size, _flags));
#else
    return ATCALL(dirfd, pathname, setattrlist(pathname, a, buf, size, flags));
#endif
}

#endif  /* __MP_LEGACY_SUPPORT_SETATTRLISTAT__ */
