/**
 * 版权声明 - Mozilla 公共许可证 2.0 版
 * Copyright (c) 2019 Paul-Louis Ageneau
 * 
 * 本源代码受Mozilla公共许可证条款约束。
 * 如果未随本文件分发MPL副本，可从 https://mozilla.org/MPL/2.0/ 获取
 */

// 防止头文件重复包含的经典宏
#ifndef RTC_COMMON_H
#define RTC_COMMON_H

// 静态库导出符号处理
#ifdef RTC_STATIC
#define RTC_CPP_EXPORT  // 静态库不需要特殊导出标记,定义一个空宏
#else // 动态库情况
#ifdef _WIN32  // Windows平台特殊处理：解决Windows平台动态库的符号可见性问题
#ifdef RTC_EXPORTS
#define RTC_CPP_EXPORT __declspec(dllexport) // 编译动态库时导出符号
#else
#define RTC_CPP_EXPORT __declspec(dllimport) // 使用动态库时导入符号
#endif
#else // 非Windows平台
#define RTC_CPP_EXPORT  // 其他平台通常不需要特殊标记
#endif
#endif

// Windows平台特定设置
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602 // 设置最低支持Windows 8（0x0602对应Win8）
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4251) // 禁用MSVC警告4251（模板类导出问题）
#endif
#endif

// WebSocket功能开关（默认启用）
#ifndef RTC_ENABLE_WEBSOCKET
#define RTC_ENABLE_WEBSOCKET 1
#endif

// 媒体功能开关（默认启用）
#ifndef RTC_ENABLE_MEDIA
#define RTC_ENABLE_MEDIA 1
#endif

// 包含C语言API定义
#include "rtc.h" // 包含C API定义

// 包含工具类头文件
#include "utils.hpp"

// 标准库头文件包含
#include <cstddef>      // 基本类型定义（size_t等）
#include <functional>   // 函数对象
#include <memory>       // 智能指针
#include <mutex>       // 互斥锁
#include <optional>    // 可选值
#include <string>      // 字符串
#include <string_view> // 字符串视图
#include <variant>     // 变体类型
#include <vector>      // 动态数组

// 定义rtc命名空间
namespace rtc {

// 标准库组件别名,使其可以不用加std前缀
using std::byte;       // 字节类型 cpp17引入的字节类型
using std::nullopt;    // 空可选值
using std::optional;   // 可选值模板
using std::shared_ptr; // 共享指针
using std::string;     // 字符串
// 避免创建临时的 std::string 对象，减少内存开销
using std::string_view;
using std::unique_ptr; // 独占指针
using std::variant;    // 一个类型安全的联合体
using std::weak_ptr;   // 弱指针

// 项目特定类型别名
using binary = std::vector<byte>;  // 二进制数据（字节向量）
/*variant表示一个类型安全的联合体（union），可以存储其模板参数列表中指定的任意一种类型的值。*/
using message_variant = variant<binary, string>; // 消息类型（二进制或字符串）

// 标准整数类型别名
using std::int16_t;    // 16位有符号整数
using std::int32_t;    // 32位有符号整数
using std::int64_t;    // 64位有符号整数
using std::int8_t;     // 8位有符号整数
using std::ptrdiff_t;  // 指针差值类型
using std::size_t;     // 大小类型
using std::uint16_t;   // 16位无符号整数
using std::uint32_t;   // 32位无符号整数
using std::uint64_t;   // 64位无符号整数
using std::uint8_t;    // 8位无符号整数

} // namespace rtc结束

#endif // 头文件结束标记（对应开头的#ifndef）