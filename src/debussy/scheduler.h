#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include <avr.h>

/*
 * <scheduler> decl
 */
typedef bool (*f_Schedule_Idle) (void* user_data);

/*
 * <scheduler> public
 */
void schd_run(f_Schedule_Idle f_idle, void* user_data);


#endif // SCHEDULER_H_INCLUDED
