/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIShell.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Remote shell command execution for HTTP UI
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HUI_SHELL_H__
#define __HUI_SHELL_H__

#include <string>

namespace HUIShell {

/**
 * @brief Initialize shell session
 * @return 0 if success, -1 if failed
 */
int InitSession();

/**
 * @brief Get current working directory
 * @return Current directory path
 */
std::string GetCurrentDir();

/**
 * @brief Get shell prompt string (e.g., "dx@WH-D-007964A:/path$")
 * @return Formatted prompt string
 */
std::string GetPrompt();

/**
 * @brief Execute shell command with session context (maintains cwd)
 * @param cmd Shell command to execute
 * @param output Command output (stdout + stderr mixed)
 * @param timeoutMs Execution timeout in milliseconds (0 = no timeout)
 * @return 0 if success, -1 if failed
 */
int ExecuteCommand(const std::string& cmd, std::string& output, int timeoutMs = 5000);

/**
 * @brief Send one command line into the persistent PTY shell.
 * @param cmd Raw command text.
 * @param promptBefore Prompt before writing the command.
 * @return 0 if success, -1 if failed.
 */
int SendCommand(const std::string& cmd, std::string& promptBefore);

/**
 * @brief Read available PTY output without blocking for command completion.
 * @param output Newly available terminal output.
 * @param promptAfter Latest prompt when the shell returns to idle.
 * @param promptReady Whether a fresh prompt was observed in this read.
 * @param waitMs Long-poll wait time in milliseconds.
 * @return 0 if success, -1 if failed.
 */
int ReadOutput(std::string& output, std::string& promptAfter, bool& promptReady, int waitMs = 200);

/**
 * @brief Send Ctrl+C into the persistent PTY shell.
 * @return 0 if success, -1 if failed.
 */
int Interrupt();

/**
 * @brief Get device profile info (hostname, uptime, kernel version)
 * @param profile JSON string containing device info
 * @return 0 if success, -1 if failed
 */
int GetDeviceProfile(std::string& profile);

/**
 * @brief Get system resource usage (CPU, memory, disk)
 * @param resources JSON string containing resource info
 * @return 0 if success, -1 if failed
 */
int GetResourceUsage(std::string& resources);

}  // namespace HUIShell

#endif // __HUI_SHELL_H__
