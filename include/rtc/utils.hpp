/**
 * Copyright (c) 2019-2021 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RTC_UTILS_H
#define RTC_UTILS_H

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <tuple>
#include <utility>

namespace rtc {

// overloaded helper 用于将多个lambda或函数对象组合成一个可重载调用的对象
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>; // 推导指引

// weak_ptr bind helper
template <typename F, typename T, typename... Args> auto weak_bind(F &&f, T *t, Args &&..._args) {
	return [bound = std::bind(f, t, _args...), weak_this = t->weak_from_this()](auto &&...args) {
		if (auto shared_this = weak_this.lock())
			return bound(args...); // 调用绑定函数
		else
			return static_cast<decltype(bound(args...))>(false);
	};
}

// scope_guard helper 在析构时自动执行指定的清理函数
class scope_guard final {
public:
	scope_guard(std::function<void()> func) : function(std::move(func)) {}
	scope_guard(scope_guard &&other) = delete;
	scope_guard(const scope_guard &) = delete;
	void operator=(const scope_guard &) = delete;

	~scope_guard() {
		if (function)
			function();
	}

private:
	std::function<void()> function;
};

// callback with built-in synchronization 带同步机制的回调类模板
template <typename... Args> class synchronized_callback {
public:
	synchronized_callback() = default;
	synchronized_callback(synchronized_callback &&cb) { *this = std::move(cb); }
	synchronized_callback(const synchronized_callback &cb) { *this = cb; }
	synchronized_callback(std::function<void(Args...)> func) { *this = std::move(func); }
	virtual ~synchronized_callback() { *this = nullptr; }

	synchronized_callback &operator=(synchronized_callback &&cb) {
		std::scoped_lock lock(mutex, cb.mutex);
		set(std::exchange(cb.callback, nullptr));
		return *this;
	}

	synchronized_callback &operator=(const synchronized_callback &cb) {
		std::scoped_lock lock(mutex, cb.mutex);
		set(cb.callback);
		return *this;
	}

	synchronized_callback &operator=(std::function<void(Args...)> func) {
		std::lock_guard lock(mutex);
		set(std::move(func));
		return *this;
	}

	bool operator()(Args... args) const {
		std::lock_guard lock(mutex);
		return call(std::move(args)...);
	}

	operator bool() const {
		std::lock_guard lock(mutex);
		return callback ? true : false;
	}

protected:
	virtual void set(std::function<void(Args...)> func) { callback = std::move(func); }
	virtual bool call(Args... args) const {
		if (!callback)
			return false;

		callback(std::move(args)...);
		return true;
	}

	std::function<void(Args...)> callback;
	mutable std::recursive_mutex mutex;
};

// callback with built-in synchronization and replay of the last missed call
template <typename... Args> // 类不能被继承 带存储功能的同步回调类模板
class synchronized_stored_callback final : public synchronized_callback<Args...> {
public:
	template <typename... CArgs>
	synchronized_stored_callback(CArgs &&...cargs)
	// 使用 std::forward 完美转发构造函数参数给基类 synchronized_callback 的构造函数
	    : synchronized_callback<Args...>(std::forward<CArgs>(cargs)...) {}
	~synchronized_stored_callback() {}

private:
	/*它的主要功能是在设置回调函数时，如果之前存储了一些参数，会立即使用这些参数调用新设置的回调函数*/
	void set(std::function<void(Args...)> func) {
		// 调用基类的 set 方法来设置回调函数
		synchronized_callback<Args...>::set(func);
		// 如果传入的回调函数有效且 stored 成员（一个 std::optional<std::tuple<Args...>> 类型）有值
		if (func && stored) {
			// 使用 std::apply 调用回调函数，并将 stored 中的参数解包传递给回调函数
			// std::move 用于移动 stored 中的参数，避免不必要的拷贝
			std::apply(func, std::move(*stored)); // 其中的*用于获取一个optional对象的value
			stored.reset(); // 调用完回调函数后，重置 stored 成员，使其变为空状态
		}
	}
	 // 调用回调函数的方法
	bool call(Args... args) const {
		// 调用基类的 call 方法尝试调用回调函数
        // 如果基类的 call 方法返回 false，说明回调函数可能未设置或调用失败
		if (!synchronized_callback<Args...>::call(args...))
			// 将传入的参数存储到 stored 成员中
            // std::move 用于移动参数，避免不必要的拷贝
			stored.emplace(std::move(args)...);
		// 调用操作完成
		return true;
	}
	// 可变的成员变量，用于存储回调函数的参数，std::tuple<Args...> 是一个元组，用于存储可变数量和类型的参数
	mutable std::optional<std::tuple<Args...>> stored;
};


// pimpl base class
template <typename T> using impl_ptr = std::shared_ptr<T>;

/*只开放接口，T是一个实现类*/
template <typename T> class CheshireCat {
public:
	CheshireCat(impl_ptr<T> impl) : mImpl(std::move(impl)) {}
	template <typename... Args>
	CheshireCat(Args... args) : mImpl(std::make_shared<T>(std::forward<Args>(args)...)) {}
	CheshireCat(CheshireCat<T> &&cc) { *this = std::move(cc); }
	CheshireCat(const CheshireCat<T> &) = delete;

	virtual ~CheshireCat() = default;

	CheshireCat &operator=(CheshireCat<T> &&cc) {
		mImpl = std::move(cc.mImpl);
		return *this;
	};
	CheshireCat &operator=(const CheshireCat<T> &) = delete;

protected:
	impl_ptr<T> impl() { return mImpl; }
	impl_ptr<const T> impl() const { return mImpl; }

private:
	impl_ptr<T> mImpl;
};

} // namespace rtc

#endif
