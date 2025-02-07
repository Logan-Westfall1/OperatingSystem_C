#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <sys_call.h>
#include <date.h>
#include <alarm.h>
#include <pcb.h>
#include <load_r3.h>



/*
* Alarm helper functions mostly, some are probably unnecessary they were just being used in my bad code I deleted
*
* I think converting the times to seconds and then comparing them is a decent way of doing it which is what most of these functions do
*
*/

int ALARM_TIME_STORE = -1;
char* ALARM_MESSAGE_STORE = NULL;

int total_seconds(int time[3])
{
    return time[0] * 3600 + time[1] * 60 + time[2];
}

int get_current_seconds(void)
{
    int currentInt[3] = { 0 };
    char* currentTime = get_time();
    unsigned char current_hour = (currentTime[0] - '0') * 10 + (currentTime[1] - '0');
    unsigned char current_minute = (currentTime[2] - '0') * 10 + (currentTime[3] - '0');
    unsigned char current_second = (currentTime[4] - '0') * 10 + (currentTime[5] - '0');

    currentInt[0] = current_hour;
    currentInt[1] = current_minute;
    currentInt[2] = current_second;
    sys_free_mem(currentTime);
    return total_seconds(currentInt);
}

int get_target_seconds(char* targetTime)
{
    

    int toks = 0;
    char* tokDate[3] = { 0 };
    char* token = strtok(targetTime, DELIMITERS);
    while (token != NULL)
    {
        tokDate[toks++] = token;
        token = strtok(NULL, DELIMITERS);
    }

    int targetInt[3] = { 0 };
    unsigned char target_hour = (tokDate[0][0] - '0') * 10 + (tokDate[0][1] - '0');
    unsigned char target_minute = (tokDate[1][0] - '0') * 10 + (tokDate[1][1] - '0');
    unsigned char target_second = (tokDate[2][0] - '0') * 10 + (tokDate[2][1] - '0');



    targetInt[0] = target_hour;
    targetInt[1] = target_minute;
    targetInt[2] = target_second;

    return total_seconds(targetInt);
}


void alarm_store(char* time, char* message){
    ALARM_TIME_STORE = get_target_seconds(time);
    ALARM_MESSAGE_STORE = message;


}


//really bad but kinds works
void alarm_process(void)
{
    int target_seconds = 0;
    char message[50];
    strncpy(message, ALARM_MESSAGE_STORE, strlen(ALARM_MESSAGE_STORE));


    if(ALARM_TIME_STORE != -1){
        target_seconds = ALARM_TIME_STORE;
        ALARM_TIME_STORE = -1;
    }

    while(target_seconds != -1){
        if(target_seconds <= get_current_seconds()){
            println(message);
            sys_req(EXIT);
        }
        sys_req(IDLE);
    }


}   

void create_alarm(char* time, char* message){

    char* current_time = get_time();

    alarm_store(time, message);
    pcb* alarm1 = pcb_setup(current_time, USER_PROCESS, 8);
    initialize_context(alarm1, alarm_process, 0);
    sys_free_mem(current_time);
}
