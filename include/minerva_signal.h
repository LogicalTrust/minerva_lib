/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_SIGNAL_H_
#define _MINERVA_SIGNAL_H_

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

extern sig_atomic_t exit_loop;
extern sig_atomic_t segv;

void minerva_signal_setup(jmp_buf *);
void minerva_signal_revert(void);

#endif /* ! _MINERVA_SIGNAL_H_ */
