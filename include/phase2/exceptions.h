#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include <pandos_types.h>

void exceptionHandler();

/**
 * @brief Implements pass up or die logic
 *
 * @param passupType
 */
void passUpOrDie(int passupType);

/**
 * @brief Get the Passed Time
 * returns the time passed since the last process started
 * and updates time of day
 *
 * @return unsigned int
 */
unsigned int getPassedTime();

/**
 * @brief updates last value of the time of day
 *  and updates the process time of the current process
 */
void updateProcessTime();

#endif /* _EXCEPTIONS_H */
