#pragma once
#include<Siv3D.hpp>
#include<type_traits>
#include<optional>

namespace util {
	template<class T, class F>
	std::result_of_t<F(T)> flatmap(s3d::Array<T> const&a, F f) {
		std::result_of_t<F(T)> tmp;
		for (auto&e : a) {
			tmp.append(f(e));
		}
		return tmp;
	}

	namespace detail {
		// https://wandbox.org/permlink/Oc3SFaTNHmG8joUn
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 1>, T&x) {
			auto&[e0] = x;
			return std::tie(e0);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 2>, T&x) {
			auto&[e0, e1] = x;
			return std::tie(e0, e1);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 3>, T&x) {
			auto&[e0, e1, e2] = x;
			return std::tie(e0, e1, e2);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 4>, T&x) {
			auto&[e0, e1, e2, e3] = x;
			return std::tie(e0, e1, e2, e3);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 5>, T&x) {
			auto&[e0, e1, e2, e3, e4] = x;
			return std::tie(e0, e1, e2, e3, e4);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 6>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5] = x;
			return std::tie(e0, e1, e2, e3, e4, e5);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 7>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 8>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 9>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 10>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 11>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 12>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 13>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 14>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 15>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14);
		}
		template<class T>
		auto to_tie(std::integral_constant<std::size_t, 16>, T&x) {
			auto&[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15] = x;
			return std::tie(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15);
		}

	}

	template<std::size_t N, class T>
	struct Record
	{
		using Index = std::integral_constant<std::size_t, N>;

		friend bool operator==(T const&lhs, T const&rhs) {
			return detail::to_tie(Index{}, lhs) == detail::to_tie(Index{}, rhs);
		}
		friend bool operator!=(T const&lhs, T const&rhs) {
			return !(lhs == rhs);
		}
		friend bool operator<(T const&lhs, T const&rhs) {
			return detail::to_tie(Index{}, lhs) < detail::to_tie(Index{}, rhs);
		}

		template<class ...Args>
		static T ctor(Args const&...args) {
			T tmp;
			detail::to_tie(Index{}, tmp) = std::make_tuple(args...);
			return tmp;
		}
	};

	template<class F1, class... Fs>
	struct overload : F1, overload<Fs...>
	{
		using F1::operator();
		using overload<Fs...>::operator();
		overload(F1 f1, Fs... fs) : F1(f1), overload<Fs...>(fs...) {}
	};

	template<class F1>
	struct overload<F1> : F1
	{
		using F1::operator();
		overload(F1 f1) : F1(f1) {}
	};

	template<class... Fs>
	overload<Fs...> make_overload(Fs... fs) {
		return overload<Fs...>(fs...);
	}

	template<class T>
	T& at_obj(s3d::Array<T>&obj_list, s3d::String const&id) {
		auto it = std::find_if(obj_list.begin(), obj_list.end(), [&](T const&obj) {return obj.id() == id; });
		return *it;
	}

	template<class T>
	T& at_model(s3d::Array<T>&obj_list, s3d::String const&id) {
		auto it = std::find_if(obj_list.begin(), obj_list.end(), [&](T const&obj) {return obj.id == id; });
		return *it;
	}
	template<class T>
	std::shared_ptr<T > & at_model(s3d::Array<std::shared_ptr<T>>&obj_list, s3d::String const&id) {
		auto it = std::find_if(obj_list.begin(), obj_list.end(), [&](std::shared_ptr<T> const&obj) {return obj->id == id; });
		return *it;
	}

	template<class T, class F>
	std::map<std::result_of_t<F(T)>, s3d::Array<T>> group_by(s3d::Array<T> const&list, F f) {
		std::map<std::result_of_t<F(T)>, s3d::Array<T>> tmp;

		for (auto &e : list) {
			auto k = f(e);
			tmp[k].push_back(e);
		}

		return tmp;

	}

	template<class...T>
	auto visit(T const&...t) {
		return [=](auto ...f) {return std::visit(make_overload(f...), t...); };
	}

	template<class T>
	auto make_vector(T const& list) -> s3d::Array<typename std::iterator_traits<decltype(begin(list))>::value_type> {
		return { begin(list) ,end(list) };
	}

	template<class T, class F>
	T max_by(s3d::Array<T> const& list, F f) {
		auto init = begin(list);

		for (auto it = std::next(init); it != list.end(); ++it) {
			init = f(*init) < f(*it) ? it : init;
		}
		return *init;
	}

	template<class T, class F>
	std::optional<T> find_if(s3d::Array<T> const& list, F f) {
		auto it = std::find_if(list.begin(), list.end(), f);
		if (it != list.end()) return  *it;
		else return std::nullopt;
	}
}
