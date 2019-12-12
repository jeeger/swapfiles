AC_PREREQ([2.69])
AC_INIT([swapfile], [0.1], [jan.seeger+swapfile@thenybble.de])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
# Add defines
AC_GNU_SOURCE
# Check for programs
AC_PROG_CC
AC_PROG_CC_STDC
# Check for headers
AC_HEADER_STDBOOL
# For RENAME_EXCHANGE flag.
AC_CHECK_HEADER([linux/fs.h],[],AC_MSG_RESULT([Missing linux/fs.h header.]))

RENAMEAT_CPP="
#include <syscall.h>
#ifndef __NR_renameat2
#error renameat2 syscall not available.
#endif
"

AC_MSG_CHECKING(renameat2() syscall is defined)
AC_PREPROC_IFELSE([AC_LANG_SOURCE([$RENAMEAT_CPP])],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_FAILURE([renameat2() syscall not defined.])])


RENAMEAT_IMPLEMENTED="
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
int main() {
    if (syscall(SYS_renameat2, 0, \"$(pwd)/one\", 0, \"$(pwd)/two\") != 0) {
       if (errno == ENOSYS) {
          return 1;
       }
    }
    return 0;
}
"

AC_MSG_CHECKING(renameat2() syscall is implemented)
AC_RUN_IFELSE([AC_LANG_SOURCE([$RENAMEAT_IMPLEMENTED])],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_WARN([renameat2() syscall not implemented, result will not work.])],
        [AC_MSG_WARN([cross-compiling might not work.])])
AC_OUTPUT
