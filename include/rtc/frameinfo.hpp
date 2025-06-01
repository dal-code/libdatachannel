/**
 * Copyright (c) 2019-2020 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RTC_FRAMEINFO_H
#define RTC_FRAMEINFO_H

#include "common.hpp"
/*提供了处理时间的工具 */
#include <chrono>

namespace rtc {
/*FrameInfo结构体用于存储媒体帧的元信息*/
struct RTC_CPP_EXPORT FrameInfo { // 
	// 使用 32 位 RTP 时间戳初始化对象
	FrameInfo(uint32_t timestamp) : timestamp(timestamp) {};
	/* 使用 C++ <chrono> 的 duration 类型初始化 
		Period = std::ratio<1> 默认单位为秒 std::ratio<1> 为1秒,std::ratio<1, 1000> → 毫秒
		通过模板构造函数，可以直接使用 100ms 或 2.5s 等直观的时间单位
	*/ 
	template<typename Period = std::ratio<1>> FrameInfo(std::chrono::duration<double, Period> timestamp) : timestampSeconds(timestamp) {};
	// 已弃用的构造函数：同时设置负载类型和时间戳（通过 [[deprecated]] 标记为不推荐使用）。
	[[deprecated]] FrameInfo(uint8_t payloadType, uint32_t timestamp) : timestamp(timestamp), payloadType(payloadType) {};
	/*传统的 32 位 RTP 时间戳,FrameInfo info1(36000);  // 假设时钟频率为90kHz，对应400ms。*/ 
	uint32_t timestamp = 0; 
	uint8_t payloadType = 0; // RTP 负载类型（标识媒体格式，如 VP8/VP9/AV1 等）。
	// timestampSeconds：高精度时间戳
	optional<std::chrono::duration<double>> timestampSeconds;
};

} // namespace rtc

#endif // RTC_FRAMEINFO_H
