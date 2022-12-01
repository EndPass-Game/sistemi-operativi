#ifndef _MACROS_H
#define _MACROS_H

#define MAX_PROC 20
#define MAX_TYPES 10
#define NULL ((void *) 0)

// terminal register defines
#define ST_READY 1
#define ST_BUSY 3
#define ST_TRANSMITTED 5

#define CMD_ACK 1
#define CMD_TRANSMIT 2

#define CHAR_OFFSET 8
#define TERM_STATUS_MASK 0xFF

#endif /* _MACROS_H */
