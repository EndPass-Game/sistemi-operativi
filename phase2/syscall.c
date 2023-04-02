#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // STATUS_KUp

#include "def-syscall.h"  // sycall codes
#include "syscall.h"


void syscallHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    if (!(old_state->status & STATUS_KUp)) {
        // TODO: simulate program trap, syscall without privilege
    }

    // see 3.5 of pandos.pdf
    unsigned int syscall_code = old_state->reg_a0;
    unsigned int a1 = old_state->reg_a1;
    unsigned int a2 = old_state->reg_a2;
    unsigned int a3 = old_state->reg_a3;

    // TODO: decidere se per le syscall è meglio passare funzioni
    // (metodo più clean, ma più lento)
    // oppure direttamente prenderle dalla memoria (più veloce)

    unsigned int result = 0;

    switch (syscall_code) {
    case SYSCALL_CREATEPROCESS:
        result = sysCreateProcess(a1, a2, a3);
        break;
    case SYSCALL_TERMPROCESS:
        sysTerminateProcess(a1);
        break;
    case SYSCALL_PASSEREN:
        sysPasseren(a1);
        break;
    case SYSCALL_VERHOGEN:
        sysVerhogen(a1);
        break;
    case SYSCALL_DOIO:
        result = sysDoIO(a1, a2);
        break;
    case SYSCALL_GETTIME:
        result = sysGetTime();
        break;
    case SYSCALL_CLOCKWAIT:
        sysClockWait();
        break;
    case SYSCALL_GETSUPPORTPTR:
        result = (unsigned int) sysGetSupportPtr();
        break;
    case SYSCALL_GETPROCESSID:
        result = sysGetProcessId(a1);
        break;
    case SYSCALL_GET_CHILDREN:
        result = sysGetChildren(a1, a2);
        break;
    default:
        // TODO: implement defalt case.
        break;
    }

    // see 7.2.3 pops for return register
    old_state->reg_v0 = result;

    // prevent infinite loop, see 3.5.10 pandos.pdf
    old_state->pc_epc += WORDLEN;

    LDST(old_state);
}

int sysCreateProcess(state_t *statep, support_t *supportp, nsd_t *ns) {
    // TODO;
    return 0;
}

void sysTerminateProcess(int pid) {
    // TODO;
}

void sysPasseren(int *semaddr) {
    // TODO;
}

void sysVerhogen(int *semaddr) {
    // TODO;
}

int sysDoIO(int *cmdAddr, int *cmdValues) {
    // TODO;
    return 0;
}

int sysGetTime(void) {
    // TODO;
}

void sysClockWait(void) {
    // TODO;
}

support_t *sysGetSupportPtr(void) {
    // TODO;
    return NULL;
}

int sysGetProcessId(int parent) {
    // TODO;
    return 0;
}

int sysGetChildren(int *children, int size) {
    // TODO;
    return 0;
}