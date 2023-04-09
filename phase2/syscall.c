#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // STATUS_KUp
#include <umps3/umps/libumps.h>  // LDST

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
        result = sysCreateProcess((state_t *) a1, (support_t *) a2, (nsd_t *) a3);
        break;
    case SYSCALL_TERMPROCESS:
        sysTerminateProcess((int) a1);
        break;
    case SYSCALL_PASSEREN:
        sysPasseren((int *) a1);
        break;
    case SYSCALL_VERHOGEN:
        sysVerhogen((int *) a1);
        break;
    case SYSCALL_DOIO:
        result = sysDoIO((int *) a1, (int *) a2);
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
        result = sysGetProcessID((int) a1);
        break;
    case SYSCALL_GET_CHILDREN:
        result = sysGetChildren((int *) a1, (int) a2);
        break;
    default:
        // Perform passupor die TODO: decide if should be own func
        // TODO: check support vector to decide if you should die
        if (/*check support == NULL*/ false) {

        } else {
            passupvector_t *passupvector = (passupvector_t *) (BIOS_EXEC_HANDLERS_ADDRS);
            // Per il nucleo ho solamente una locazione in cui salvare lo stato
            STST((void *) BIOS_DATA_PAGE_BASE);
            ((void (*)())passupvector->exception_handler)();
        }
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
    return 0;
}

void sysClockWait(void) {
    // TODO;
}

support_t *sysGetSupportPtr(void) {
    // TODO;
    return NULL;
}

int sysGetProcessID(int parent) {
    // TODO;
    return 0;
}

int sysGetChildren(int *children, int size) {
    // TODO;
    return 0;
}