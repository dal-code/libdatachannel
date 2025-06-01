/**
 * Copyright (c) 2019-2021 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RTC_IMPL_CHANNEL_H
#define RTC_IMPL_CHANNEL_H

// 该文件可能定义了一些通用的类型、常量或函数
#include "common.hpp"
// 该文件可能定义了消息相关的类型
#include "message.hpp"

// 用于使用原子类型，保证多线程环境下的数据操作安全
#include <atomic>
// 用于使用函数对象、函数指针等功能
#include <functional>

// 定义命名空间 rtc::impl，将相关的类和函数封装在该命名空间下，避免命名冲突
namespace rtc::impl {

// 定义一个抽象基类 Channel，包含了一些纯虚函数和普通成员函数
struct Channel {
    // 纯虚函数，用于接收消息，返回一个可选的消息变体对象
    // 返回一个optional对象，要么包含一个message_variant类型的值
    virtual optional<message_variant> receive() = 0; 
    // 纯虚函数，用于查看消息，返回一个可选的消息变体对象，但不会从消息队列中移除该消息
    // 如果有消息可查看，则返回消息变体对象；否则返回空的 optional 对象
    virtual optional<message_variant> peek() = 0;
    // 纯虚函数，用于获取当前可接收的消息数量
    virtual size_t availableAmount() const = 0;

	/*触发回调函数的几个函数，封装调用了下面的回调函数*/
    // 虚函数，触发通道打开事件的回调函数
    virtual void triggerOpen();
    // 虚函数，触发通道关闭事件的回调函数
    virtual void triggerClosed();
    // 虚函数，触发通道错误事件的回调函数，参数为错误信息字符串
    virtual void triggerError(string error);
    // 虚函数，触发通道有可用消息事件的回调函数，参数为可用消息的数量
    virtual void triggerAvailable(size_t count);
    // 虚函数，触发通道缓冲区大小变化事件的回调函数，参数为当前缓冲区的大小
    virtual void triggerBufferedAmount(size_t amount);

    // 虚函数，用于刷新待处理的消息
    virtual void flushPendingMessages();
    // 重置通道打开事件的回调函数
    void resetOpenCallback();
    // 重置所有事件的回调函数
    void resetCallbacks();

	/*重要的回调函数，被触发函数调用，在具体文件中进行定义 */
    // 同步存储的回调函数对象，用于处理通道打开事件
    synchronized_stored_callback<> openCallback;
    // 同步存储的回调函数对象，用于处理通道关闭事件
    synchronized_stored_callback<> closedCallback;
    // 同步存储的回调函数对象，用于处理通道错误事件，参数为错误信息字符串
    synchronized_stored_callback<string> errorCallback;
    // 同步存储的回调函数对象，用于处理通道有可用消息事件
    synchronized_stored_callback<> availableCallback;
    // 同步存储的回调函数对象，用于处理通道缓冲区大小低于阈值事件
    synchronized_stored_callback<> bufferedAmountLowCallback;

    // 同步回调函数对象，用于处理接收到的消息，参数为消息变体对象
    synchronized_callback<message_variant> messageCallback;

    // 原子类型的变量，用于记录当前通道的缓冲区大小
    std::atomic<size_t> bufferedAmount = 0;
    // 原子类型的变量，用于记录通道缓冲区大小的低阈值
    std::atomic<size_t> bufferedAmountLowThreshold = 0;

protected:
    // 原子类型的布尔变量，用于标记通道打开事件是否已经触发
    std::atomic<bool> mOpenTriggered = false;
};

} // namespace rtc::impl

#endif
