/* MP support header */
#include "MacportsLegacySupport.h"
#if __MP_LEGACY_SUPPORT_PTHREAD_THREADID_NP__

#include <errno.h>
#include <pthread.h>
#include <mach/kern_return.h>
#include <mach/thread_info.h>
#include <mach/thread_act.h>

int pthread_threadid_np(pthread_t thread, __uint64_t *thread_id) {

    if (thread_id == NULL) {
        return EINVAL;
    }

    if (thread == NULL) {
        thread = pthread_self();
        
        if (thread == NULL ) {
          return ESRCH;
        }
    }

    mach_port_name_t port = pthread_mach_thread_np(thread);

    thread_identifier_info_data_t info;
    mach_msg_type_number_t info_count = THREAD_IDENTIFIER_INFO_COUNT;

    kern_return_t kr = thread_info(port,
                                   THREAD_IDENTIFIER_INFO,
                                   (thread_info_t)&info,
                                   &info_count);
    if(kr != KERN_SUCCESS) {
        return ESRCH;
    } else {
        *thread_id = info.thread_id;
        return 0;
    }
}

#endif