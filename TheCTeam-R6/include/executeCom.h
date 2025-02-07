/**
 @file executeCom.h
 @brief Kernel function for executing all commands to the command handler.
*/

/**
 * @brief Parses and executes the given command.
 *
 * This function takes a user command string, tokenizes it to extract the command and arguments,
 * and executes the corresponding functionality based on the command (shutdown, help, date, version).
 *
 * @param buf The input command string from the user.
 * @return 0 on successful execution, 1 if shutdown is confirmed.
 */
int execute_com(char* buf);

