/**
 * Copyright (c) 2019 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
/*
表示网络通信中的候选地址(如 ICE 候选地址)
*/
#ifndef RTC_CANDIDATE_H
#define RTC_CANDIDATE_H

#include "common.hpp"

#include <string>

namespace rtc {

class RTC_CPP_EXPORT Candidate {
public:
	enum class Family { Unresolved, Ipv4, Ipv6 }; // Unresolved表示未解析
	// 候选地址类型:未知、主机地址、服务器反射地址（通过 NAT 反射得到）、对等反射、中继地址
	enum class Type { Unknown, Host, ServerReflexive, PeerReflexive, Relayed };
	// 传输协议类型: 未知、UDP、tcp主动、tcp被动、tcp同时打开、tcp模式未知
	enum class TransportType { Unknown, Udp, TcpActive, TcpPassive, TcpSo, TcpUnknown };

	Candidate();
	/*通过候选地址字符串（
	如 "candidate:1 1 udp 2130706431 192.168.1.1 51413 typ host"）初始化对象。*/ 
	Candidate(string candidate);
	// Media ID 用于标识媒体流，关联多个候选地址到同一媒体。
	Candidate(string candidate, string mid);

	void hintMid(string mid); // 设置或提示 mid 值。
	/*修改地址信息 */
	void changeAddress(string addr);
	void changeAddress(string addr, uint16_t port);
	void changeAddress(string addr, string service); // 地址 + 服务名（如 "80/tcp"）。

	enum class ResolveMode { Simple, Lookup }; // 解析模式: Lookup 可能涉及 DNS 查找。
	bool resolve(ResolveMode mode = ResolveMode::Simple); // 解析候选地址
	/*查询方法 候选地址类型、传输协议类型、优先级、原始字符串、mid*/
	Type type() const;
	TransportType transportType() const;
	uint32_t priority() const;
	string candidate() const;
	string mid() const;
	/*类型转换运算符（Conversion Operator），用于将当前类的对象转换为 std::string 类型*/
	operator string() const; // 将候选地址转换为字符串表示，方便输出或序列化。

	bool operator==(const Candidate &other) const;
	bool operator!=(const Candidate &other) const;
	/*状态查询：判断地址是否已解析、返回地址族、解析后的地址和端口*/
	bool isResolved() const;
	Family family() const;
	optional<string> address() const;
	optional<uint16_t> port() const;

private:
	// 私有方法，用于解析候选地址字符串（如 ICE 候选地址格式）。
	void parse(string candidate);

	string mFoundation;       // 候选地址基础标识（如用户名片段）
    uint32_t mComponent, mPriority; // 组件 ID（如传输层组件）、优先级
    string mTypeString, mTransportString; // 原始类型和传输协议字符串
    Type mType;                // 解析后的类型枚举值
    TransportType mTransportType; // 解析后的传输协议枚举值
    string mNode, mService;    // 节点名（用于 DNS 解析）、服务名
    string mTail;              // 候选地址尾部附加信息

    optional<string> mMid;      // Media ID

    // 解析后存储的地址信息
    Family mFamily;            // 地址族（IPv4/IPv6）
    string mAddress;           // 解析后的 IP 地址
    uint16_t mPort;            // 解析后的端口号
};
//重载 << 运算符，支持将 Candidate 对象、枚举值直接输出到流（如 std::cout），方便调试和日志记录。
RTC_CPP_EXPORT std::ostream &operator<<(std::ostream &out, const Candidate &candidate);
RTC_CPP_EXPORT std::ostream &operator<<(std::ostream &out, const Candidate::Type &type);
RTC_CPP_EXPORT std::ostream &operator<<(std::ostream &out,
                                        const Candidate::TransportType &transportType);
										
} // namespace rtc

#endif
