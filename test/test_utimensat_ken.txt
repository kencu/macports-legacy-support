#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <paths.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
#include <darwintest.h>
#include <darwintest_utils.h>
*/

#define FILENAME "utimensat"
#define T_LOG printf

#define T_ASSERT_POSIX_ZERO(a,b)    \
                                    \
    if ((a) != 0) {                 \
        printf("assert zero failed"); \
        if (b != NULL) {              \
            printf(b);          \
        };                          \
        return 1;                   \
    }

#define T_ASSERT_POSIX_SUCCESS(a,b) \
                                    \
    if ((a) < 0) {                  \
        printf("assert success failed"); \
        if (b != NULL) {              \
            printf(b);          \
        };                          \
        return 1;                   \
    }

#define T_ASSERT_GE(a,b,c)    \
                                    \
    if (!(a >= b)) {                \
        printf("assert GE failed {%ld, %ld} ", a, b); \
        if (c != NULL) {              \
            printf(c);          \
        };                          \
		printf("\n");				\
        return 1;                   \
    }

#define T_ASSERT_EQ(a,b,c)    \
                                    \
    if (!(a == b)) {                \
        printf("assert EQ failed {%ld, %ld} ", a, b); \
        if (c != NULL) {              \
            printf(c);          \
        };                          \
		printf("\n");				\
        return 1;                   \
    }


static const struct timespec tptr[][2] = {
    { { 0x12345678, 987654321 }, { 0x15263748, 123456789 }, },

    { { 0, UTIME_NOW }, { 0x15263748, 123456789 }, },
    { { 0x12345678, 987654321 }, { 0, UTIME_NOW }, },
    { { 0, UTIME_NOW }, { 0, UTIME_NOW }, },

    { { 0, UTIME_OMIT }, { 0x15263748, 123456789 }, },
    { { 0x12345678, 987654321 }, { 0, UTIME_OMIT }, },
 /*   { { 0, UTIME_OMIT }, { 0, UTIME_OMIT }, }, */

    { { 0, UTIME_NOW }, { 0, UTIME_OMIT }, },
    { { 0, UTIME_OMIT }, { 0, UTIME_NOW }, },
};

int main()
{

    struct stat pre_st, post_st;
    int fd;

    T_ASSERT_POSIX_SUCCESS((fd = open(FILENAME, O_CREAT|O_RDWR, 0644)), NULL);
    T_ASSERT_POSIX_ZERO(close(fd), NULL);

/*    fd = open(FILENAME, O_CREAT|O_RDWR, 0644);
    close(fd);
*/

    for (size_t i = 0; i < sizeof(tptr)/sizeof(tptr[0]); i++) {
        T_LOG("=== {%ld, %ld} {%ld, %ld} ===\n", 
                tptr[i][0].tv_sec, tptr[i][0].tv_nsec,
                tptr[i][1].tv_sec, tptr[i][1].tv_nsec);

        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);

        T_ASSERT_POSIX_ZERO(stat(FILENAME, &pre_st), "failed 1");
        T_ASSERT_POSIX_ZERO(utimensat(AT_FDCWD, FILENAME, tptr[i], 0), "failed 2");
        T_ASSERT_POSIX_ZERO(stat(FILENAME, &post_st), NULL);

        if (tptr[i][0].tv_nsec == UTIME_NOW) {
            T_ASSERT_GE(post_st.st_atimespec.tv_sec, now.tv_sec, NULL);
        } else if (tptr[i][0].tv_nsec == UTIME_OMIT) {
            T_ASSERT_EQ(post_st.st_atimespec.tv_sec, pre_st.st_atimespec.tv_sec, "failed 1");
            T_ASSERT_EQ(post_st.st_atimespec.tv_nsec, pre_st.st_atimespec.tv_nsec, "failed 2");
        } else {
            /* T_ASSERT_EQ(post_st.st_atimespec.tv_sec, tptr[i][0].tv_sec, "failed 3"); */
            /* T_ASSERT_EQ(post_st.st_atimespec.tv_nsec, tptr[i][0].tv_nsec, "failed 4"); */
        }

        if (tptr[i][1].tv_nsec == UTIME_NOW) {
            T_ASSERT_GE(post_st.st_mtimespec.tv_sec, now.tv_sec, NULL);
        } else if (tptr[i][1].tv_nsec == UTIME_OMIT) {
            T_ASSERT_EQ(post_st.st_mtimespec.tv_sec, pre_st.st_mtimespec.tv_sec, "failed 5");
            T_ASSERT_EQ(post_st.st_mtimespec.tv_nsec, pre_st.st_mtimespec.tv_nsec, "failed 6");
        } else {
            T_ASSERT_EQ(post_st.st_mtimespec.tv_sec, tptr[i][1].tv_sec, "failed 7");
           /* T_ASSERT_EQ(post_st.st_mtimespec.tv_nsec, tptr[i][1].tv_nsec, "failed 8"); */
        }
    }
}
