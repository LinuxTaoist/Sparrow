/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogConfigKeys.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log configuration loader.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __LOG_CONFIG_KEYS_H__
#define __LOG_CONFIG_KEYS_H__

#define LOG_CONFIG_MODULE_DEFAULT           "default"
#define LOG_CONFIG_OUTPUT_PREFIX            "output."

#define LOG_CONFIG_KEY_FRAME_LENGTH_BYTES   "frame_length_bytes"
#define LOG_CONFIG_KEY_ENABLED              "enabled"
#define LOG_CONFIG_KEY_LEVEL                "level"
#define LOG_CONFIG_KEY_OUTPUT               "output"
#define LOG_CONFIG_KEY_FILE_NAME            "file_name"
#define LOG_CONFIG_KEY_FILE_NAME_FORMAT     "file_name_format"
#define LOG_CONFIG_KEY_LOG_DIR              "log_dir"
#define LOG_CONFIG_KEY_FILE_NUM             "file_num"
#define LOG_CONFIG_KEY_FILE_CAPACITY_MB     "file_capacity_mb"
#define LOG_CONFIG_KEY_FLUSH_COUNT          "flush_count"
#define LOG_CONFIG_KEY_FLUSH_INTERVAL_MS    "flush_interval_ms"
#define LOG_CONFIG_KEY_PACK_ENABLED         "pack_enabled"
#define LOG_CONFIG_KEY_PACK_NAME            "pack_name"
#define LOG_CONFIG_KEY_PACK_DIR             "pack_dir"
#define LOG_CONFIG_KEY_PACK_NAME_FORMAT     "pack_name_format"
#define LOG_CONFIG_KEY_PACK_TYPE            "pack_type"
#define LOG_CONFIG_KEY_PACK_COMMAND         "pack_command"
#define LOG_CONFIG_KEY_PACK_COMMAND_OVERRIDE "pack_command_override"
#define LOG_CONFIG_KEY_PACK_FILE_NUM        "pack_file_num"
#define LOG_CONFIG_KEY_PACK_TOTAL_SIZE_MB   "pack_total_size_mb"

#define LOG_CONFIG_DEFAULT_FILE_NAME        "sprlog.log"
#define LOG_CONFIG_DEFAULT_PACK_NAME        "sprlog.log.pack"
#define LOG_CONFIG_DEFAULT_LOG_DIR          "/tmp/sprlog"
#define LOG_CONFIG_DEFAULT_PACK_DIR         ""

#define LOG_CONFIG_VALUE_TRUE               "true"
#define LOG_CONFIG_VALUE_FALSE              "false"
#define LOG_CONFIG_VALUE_DEBUG              "debug"
#define LOG_CONFIG_VALUE_INFO               "info"
#define LOG_CONFIG_VALUE_WARN               "warn"
#define LOG_CONFIG_VALUE_ERROR              "error"
#define LOG_CONFIG_VALUE_STDOUT             "stdout"
#define LOG_CONFIG_VALUE_FILE               "file"

#endif // __LOG_CONFIG_KEYS_H__
