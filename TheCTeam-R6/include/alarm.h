/**
 * @file alarm.h
 * @brief User functions and definitions for alarm commands
 */

#ifndef ALARM_H
#define ALARM_H

#include <processes.h>  // For pcb type

/**
 * @struct alarm_params
 * @brief Structure for storing alarm parameters
 *
 * This structure holds the necessary parameters for an alarm, including
 * the target time in seconds since midnight, the message to display when
 * the alarm triggers, and the name of the alarm process.
 */
typedef struct {
    int target_seconds;    /**< Target time in seconds since midnight */
    char* message;         /**< Message to display when alarm triggers */
    char name[32];         /**< Name of the alarm process */
} alarm_params;

/**
 * @brief Calculates total seconds from hours, minutes, and seconds
 *
 * This function takes an array containing hours, minutes, and seconds,
 * and calculates the total number of seconds since midnight.
 *
 * @param time An array of three integers: [hours, minutes, seconds]
 * @return The total seconds since midnight
 */
int total_seconds(int time[3]);

/**
 * @brief Retrieves the current time in seconds since midnight
 *
 * This function obtains the current system time and converts it
 * into the total number of seconds that have elapsed since midnight.
 *
 * @return The current time in seconds since midnight
 */
int get_current_seconds(void);

/**
 * @brief Converts a target time string to total seconds
 *
 * Parses a time string formatted as "HH:MM:SS" and converts it
 * into the total number of seconds since midnight.
 *
 * @param targetTime A string representing the target time in "HH:MM:SS" format
 * @return The target time in seconds, or -1 if the format is invalid
 */
int get_target_seconds(char* targetTime);

/**
 * @brief Stores the alarm time and message for later processing
 *
 * This function saves the target time and message provided by the user.
 * These values are used by the alarm process to determine when to trigger.
 *
 * @param time A string representing the target time in "HH:MM:SS" format
 * @param message The message to display when the alarm triggers
 */
void alarm_store(char* time, char* message);

/**
 * @brief The alarm process that waits and triggers the alarm
 *
 * This function represents the alarm process. It continuously checks
 * the current time against the stored target time and displays the
 * message when the target time is reached.
 */
void alarm_process(void);

/**
 * @brief Creates and initializes a new alarm process
 *
 * This function initiates a new alarm by storing the target time and message,
 * generating a unique process name, setting up a new Process Control Block (PCB),
 * and initializing the process context to run the alarm process.
 *
 * @param targetTime A string representing the target time in "HH:MM:SS" format
 * @param message The message to display when the alarm triggers
 */
void create_alarm(char* targetTime, char* message);

#endif  // ALARM_H
