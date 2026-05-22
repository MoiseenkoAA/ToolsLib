
// for ararm'ed function's returns.

//#ifdef STAND_ALONE
#include <signal.h>
#include <stdio.h>
#include <time.h>
//#endif

#ifndef OS_RESTARTING_SIGNAL
/*************************************************
*          Set up restarting signal              *
*************************************************/

/* This function has the same functionality as the ANSI C signal() function,
except that it arranges that, if the signal happens during a system call, the
system call gets restarted. (Also, it doesn't return a result.) Different
versions of Unix have different defaults, and different ways of setting up a
restarting signal handler. If the functionality is not available, the signal
should be set to be ignored. This function is used only for catching SIGUSR1.
*/

void
os_restarting_signal(int sig, void (*handler)(int))
{
    /* Many systems have the SA_RESTART sigaction for specifying that a signal
should restart system calls. These include SunOS5, AIX, BSDI, IRIX, FreeBSD,
OSF1, Linux and HP-UX 10 (but *not* HP-UX 9). */

#ifdef SA_RESTART
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = SA_RESTART;
    sigaction(sig, &act, NULL);

#ifdef STAND_ALONE
    printf("Used SA_RESTART\n");
#endif

    /* SunOS4 and Ultrix default to non-interruptable signals, with SV_INTERRUPT
for making them interruptable. This seems to be a dying fashion. */

#elif defined SV_INTERRUPT
    signal(sig, handler);

#ifdef STAND_ALONE
    printf("Used default signal()\n");
#endif


    /* If neither SA_RESTART nor SV_INTERRUPT is available we don't know how to
set up a restarting signal, so simply suppress the facility. */

#else
    signal(sig, SIG_IGN);

#ifdef STAND_ALONE
    printf("Used SIG_IGN\n");
#endif

#endif
}

#endif  /* OS_RESTARTING_SIGNAL */



#ifndef OS_NON_RESTARTING_SIGNAL
/*************************************************
*          Set up non-restarting signal          *
*************************************************/

/* This function has the same functionality as the ANSI C signal() function,
except that it arranges that, if the signal happens during a system call, the
system call gets interrupted. (Also, it doesn't return a result.) Different
versions of Unix have different defaults, and different ways of setting up a
non-restarting signal handler. For systems for which we don't know what to do,
just use the normal signal() function and hope for the best. */

void
os_non_restarting_signal(int sig, void (*handler)(int))
{
    /* Many systems have the SA_RESTART sigaction for specifying that a signal
should restart system calls. These include SunOS5, AIX, BSDI, IRIX, FreeBSD,
OSF1, Linux and HP-UX 10 (but *not* HP-UX 9). */

#ifdef SA_RESTART
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(sig, &act, NULL);

#ifdef STAND_ALONE
    printf("Used sigaction() with flags = 0\n");
#endif

    /* SunOS4 and Ultrix default to non-interruptable signals, with SV_INTERRUPT
for making them interruptable. This seems to be a dying fashion. */

#elif defined SV_INTERRUPT
    struct sigvec sv;
    sv.sv_handler = handler;
    sv.sv_flags = SV_INTERRUPT;
    sv.sv_mask = -1;
    sigvec(sig, &sv, NULL);

#ifdef STAND_ALONE
    printf("Used sigvec() with flags = SV_INTERRUPT\n");
#endif

    /* If neither SA_RESTART nor SV_INTERRUPT is available we don't know how to
set up a restarting signal, so just use the standard signal() function. */

#else
    signal(sig, handler);

#ifdef STAND_ALONE
    printf("Used default signal()\n");
#endif

#endif
}

#endif  /* OS_NON_RESTARTING_SIGNAL */


