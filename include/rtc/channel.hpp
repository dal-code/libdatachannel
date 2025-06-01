/**
 * Copyright (c) 2019-2021 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RTC_CHANNEL_H
#define RTC_CHANNEL_H

#include "common.hpp"

#include <atomic>
#include <functional>

namespace rtc {

// 实现细节命名空间，前向声明Channel结构体，用于 Pimpl 模式。
namespace impl {
struct Channel; // 在impl命名空间中，Channel已经被实现了，这里通过继承直接使用
}
/*采用了 Pimpl（Pointer to Implementation）模式来隐藏实现细节
 */
class RTC_CPP_EXPORT Channel : private CheshireCat<impl::Channel> {
public:
	virtual ~Channel();
	/*关闭通道、发送数据*/
	virtual void close() = 0;
	virtual bool send(message_variant data) = 0; // returns false if buffered
	virtual bool send(const byte *data, size_t size) = 0;
	/*查询通道的状态、单次发送的最大消息大小，返回当前缓冲待发送的数据大小*/
	virtual bool isOpen() const = 0;
	virtual bool isClosed() const = 0;
	virtual size_t maxMessageSize() const; // max message size in a call to send
	virtual size_t bufferedAmount() const; // total size buffered to send
	/*回调函数：通道打开、关闭、错误、消息接收*/
	void onOpen(std::function<void()> callback);
	void onClosed(std::function<void()> callback);
	void onError(std::function<void(string error)> callback);

	void onMessage(std::function<void(message_variant data)> callback);
	void onMessage(std::function<void(binary data)> binaryCallback,
	               std::function<void(string data)> stringCallback);

	void onBufferedAmountLow(std::function<void()> callback);
	void setBufferedAmountLowThreshold(size_t amount);
	/*重置所有回调函数*/
	void resetCallbacks();

	// Extended API 接收或查看消息，仅在未设置 onMessage 回调时可用。
	optional<message_variant> receive(); // only if onMessage unset
	optional<message_variant> peek();    // only if onMessage unset
	size_t availableAmount() const;      // total size available to receive 返回可用待接收数据的大小。
	void onAvailable(std::function<void()> callback); // 设置有数据可用时的回调函数。

protected:
	Channel(impl_ptr<impl::Channel> impl);
};

} // namespace rtc

#endif // RTC_CHANNEL_H
