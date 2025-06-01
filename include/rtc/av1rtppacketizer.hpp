/**
 * Copyright (c) 2023 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
/* 将 AV1 视频编码数据 按照 RTP（Real-time Transport Protocol）协议 进行打包。
AV1 是一种开源的视频编码格式，而 RTP 是实时音视频传输的基础协议，
该类的主要功能是将 AV1 视频帧分割成适合网络传输的 RTP 数据包，
并处理相关的时序和分片逻辑。*/
#ifndef RTC_AV1_RTP_PACKETIZER_H
#define RTC_AV1_RTP_PACKETIZER_H

#if RTC_ENABLE_MEDIA // 默认enable

#include "mediahandler.hpp"
#include "nalunit.hpp"
#include "rtppacketizer.hpp"

namespace rtc {

// RTP packetization of AV1 payload
class RTC_CPP_EXPORT AV1RtpPacketizer final : public RtpPacketizer {
public:
	inline static const uint32_t ClockRate = VideoClockRate;
	[[deprecated("Use ClockRate")]] inline static const uint32_t defaultClockRate = ClockRate;

	// Define how OBUs are seperated in a AV1 Sample
	enum class Packetization {
		Obu = RTC_OBU_PACKETIZED_OBU,
		TemporalUnit = RTC_OBU_PACKETIZED_TEMPORAL_UNIT,
	};

	// Constructs AV1 payload packetizer with given RTP configuration.
	// @note RTP configuration is used in packetization process which may change some configuration
	// properties such as sequence number.
	AV1RtpPacketizer(Packetization packetization, shared_ptr<RtpPacketizationConfig> rtpConfig,
	                 size_t maxFragmentSize = DefaultMaxFragmentSize);

private:
	static std::vector<binary> extractTemporalUnitObus(const binary &data);

	std::vector<binary> fragment(binary data) override;
	std::vector<binary> fragmentObu(const binary &data);

	const Packetization mPacketization;
	const size_t mMaxFragmentSize;

	std::unique_ptr<binary> mSequenceHeader;
};

// For backward compatibility, do not use
using AV1PacketizationHandler [[deprecated("Add AV1RtpPacketizer directly")]] = PacketizationHandler;

} // namespace rtc

#endif /* RTC_ENABLE_MEDIA */

#endif /* RTC_AV1_RTP_PACKETIZER_H */
