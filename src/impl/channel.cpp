/**
 * Copyright (c) 2019 - 2021 Paul - Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "channel.hpp"
// 包含自定义的 internals 头文件，可能包含了一些内部实现细节的定义
#include "internals.hpp"

namespace rtc::impl {

// 触发 Channel 打开事件的函数
void Channel::triggerOpen() {
    // 标记 Channel 打开事件已触发
    mOpenTriggered = true;
    try {
        openCallback(); // 调用打开事件的回调函数
    } catch (const std::exception &e) {
        // 若回调函数抛出异常，记录警告日志，输出异常信息
        PLOG_WARNING << "Uncaught exception in callback: " << e.what();
    }
    // 刷新待处理的消息
    flushPendingMessages();
}

// 触发 Channel 关闭事件的函数
void Channel::triggerClosed() {
    try {
        // 调用关闭事件的回调函数
        closedCallback();
    } catch (const std::exception &e) {
        // 若回调函数抛出异常，记录警告日志，输出异常信息
        PLOG_WARNING << "Uncaught exception in callback: " << e.what();
    }
}

// 触发 Channel 错误事件的函数，接收一个错误信息字符串作为参数
void Channel::triggerError(string error) {
    try {
        // 调用错误事件的回调函数，并将错误信息移动传递
        errorCallback(std::move(error));
    } catch (const std::exception &e) {
        // 若回调函数抛出异常，记录警告日志，输出异常信息
        PLOG_WARNING << "Uncaught exception in callback: " << e.what();
    }
}

// 触发 Channel 数据可用事件的函数，接收一个表示可用数据数量的参数
void Channel::triggerAvailable(size_t count) {
    // 如果可用数据数量为 1
    if (count == 1) {
        try {
            // 调用数据可用事件的回调函数
            availableCallback();
        } catch (const std::exception &e) {
            // 若回调函数抛出异常，记录警告日志，输出异常信息
            PLOG_WARNING << "Uncaught exception in callback: " << e.what();
        }
    }
    // 刷新待处理的消息
    flushPendingMessages();
}

// 触发 Channel 缓冲区大小变化事件的函数，接收一个表示当前缓冲区大小的参数
void Channel::triggerBufferedAmount(size_t amount) {
    // 原子地交换当前缓冲区大小，并获取之前的缓冲区大小
    size_t previous = bufferedAmount.exchange(amount);
    // 原子地加载缓冲区低阈值
    size_t threshold = bufferedAmountLowThreshold.load();
    // 如果之前的缓冲区大小大于阈值，而当前缓冲区大小小于等于阈值
    if (previous > threshold && amount <= threshold) {
        try {
            // 调用缓冲区低阈值事件的回调函数
            bufferedAmountLowCallback();
        } catch (const std::exception &e) {
            // 若回调函数抛出异常，记录警告日志，输出异常信息
            PLOG_WARNING << "Uncaught exception in callback: " << e.what();
        }
    }
}

// 刷新待处理消息的函数
void Channel::flushPendingMessages() {
    // 如果 Channel 打开事件未触发，直接返回
    if (!mOpenTriggered)
        return;
    // 只要消息回调函数存在
    while (messageCallback) {
        // 尝试接收下一条消息
        auto next = receive();
        // 如果没有接收到消息，跳出循环
        if (!next)
            break;
        try {
            messageCallback(*next); // 调用消息回调函数，处理接收到的消息
        } catch (const std::exception &e) {
            // 若回调函数抛出异常，记录警告日志，输出异常信息
            PLOG_WARNING << "Uncaught exception in callback: " << e.what();
        }
    }
}

// 重置打开事件回调函数的函数
void Channel::resetOpenCallback() {
    // 标记 Channel 打开事件未触发
    mOpenTriggered = false;
    // 将打开事件回调函数置为空
    openCallback = nullptr;
}

// 重置所有回调函数的函数
void Channel::resetCallbacks() {
    // 标记 Channel 打开事件未触发
    mOpenTriggered = false;
    // 将打开事件回调函数置为空
    openCallback = nullptr;
    // 将关闭事件回调函数置为空
    closedCallback = nullptr;
    // 将错误事件回调函数置为空
    errorCallback = nullptr;
    // 将数据可用事件回调函数置为空
    availableCallback = nullptr;
    // 将缓冲区低阈值事件回调函数置为空
    bufferedAmountLowCallback = nullptr;
    // 将消息回调函数置为空
    messageCallback = nullptr;
}

} // namespace rtc::impl