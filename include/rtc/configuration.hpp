/**
 * Copyright (c) 2019 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*与ICE配置相关的结构体和枚举类型 */
#ifndef RTC_ICE_CONFIGURATION_H
#define RTC_ICE_CONFIGURATION_H

#include "common.hpp"

#include <vector>

namespace rtc {
// Windows 的 DLL 需要显式声明哪些类/函数可以暴露给外部使用，否则默认不导出。
struct RTC_CPP_EXPORT IceServer {
	enum class Type { Stun, Turn }; // ICE的两个服务器类型
	enum class RelayType { TurnUdp, TurnTcp, TurnTls }; // TURN服务器的中继类型(UDP/TCP/TLS)

	// Any type
	IceServer(const string &url);

	// STUN 服务器专用的构造函数，分别使用端口号或服务名。
	IceServer(string hostname_, uint16_t port_);
	IceServer(string hostname_, string service_);

	// TURN 需要用户名和密码，可选指定中继类型(默认为UDP)。
	IceServer(string hostname_, uint16_t port, string username_, string password_,
	          RelayType relayType_ = RelayType::TurnUdp);
	IceServer(string hostname_, string service_, string username_, string password_,
	          RelayType relayType_ = RelayType::TurnUdp);
	// 主机名、端口、类型、用户名、密码和中继类型。
	string hostname;
	uint16_t port;
	Type type;
	string username;
	string password;
	RelayType relayType;
};
/*代理服务器配置 */
struct RTC_CPP_EXPORT ProxyServer {
	enum class Type { Http, Socks5 };

	ProxyServer(const string &url);

	ProxyServer(Type type_, string hostname_, uint16_t port_);
	ProxyServer(Type type_, string hostname_, uint16_t port_, string username_, string password_);
	// 类型、主机名、端口，以及可选的用户名和密码
	Type type;
	string hostname;
	uint16_t port;
	optional<string> username;
	optional<string> password;
};
/*定义证书类型枚举*/ 
enum class CertificateType {
	Default = RTC_CERTIFICATE_DEFAULT, // ECDSA
	Ecdsa = RTC_CERTIFICATE_ECDSA,
	Rsa = RTC_CERTIFICATE_RSA
};
/*定义传输策略枚举(允许所有传输或仅中继)*/
enum class TransportPolicy { All = RTC_TRANSPORT_POLICY_ALL, Relay = RTC_TRANSPORT_POLICY_RELAY };

struct RTC_CPP_EXPORT Configuration {
	// ICE settings
	std::vector<IceServer> iceServers;
	optional<ProxyServer> proxyServer; // libnice only
	optional<string> bindAddress;      // libjuice only, default any

	// Options
	CertificateType certificateType = CertificateType::Default;
	TransportPolicy iceTransportPolicy = TransportPolicy::All;
	bool enableIceTcp = false;    // libnice only
	bool enableIceUdpMux = false; // libjuice only
	bool disableAutoNegotiation = false;
	bool disableAutoGathering = false;
	bool forceMediaTransport = false;
	bool disableFingerprintVerification = false;

	// Port range
	uint16_t portRangeBegin = 1024;
	uint16_t portRangeEnd = 65535;

	// Network MTU  可选的MTU(最大传输单元)设置。
	optional<size_t> mtu;

	// Local maximum message size for Data Channels
	optional<size_t> maxMessageSize; // 数据通道的最大消息大小。

	// Certificates and private keys 证书和私钥文件相关配置。
	optional<string> certificatePemFile;
	optional<string> keyPemFile;
	optional<string> keyPemPass;
};

#ifdef RTC_ENABLE_WEBSOCKET // 只有在启用WebSocket时才包含以下内容。

struct WebSocketConfiguration {
	bool disableTlsVerification = false; // if true, don't verify the TLS certificate
	optional<ProxyServer> proxyServer;   // only non-authenticated http supported for now
	std::vector<string> protocols;
	optional<std::chrono::milliseconds> connectionTimeout; // zero to disable
	optional<std::chrono::milliseconds> pingInterval;      // zero to disable
	optional<int> maxOutstandingPings;
	optional<string> caCertificatePemFile;
	optional<string> certificatePemFile;
	optional<string> keyPemFile;
	optional<string> keyPemPass;
	optional<size_t> maxMessageSize;
};

struct WebSocketServerConfiguration {
	uint16_t port = 8080;
	bool enableTls = false;
	optional<string> certificatePemFile;
	optional<string> keyPemFile;
	optional<string> keyPemPass;
	optional<string> bindAddress;
	optional<std::chrono::milliseconds> connectionTimeout;
	optional<size_t> maxMessageSize;
};

#endif

} // namespace rtc

#endif
