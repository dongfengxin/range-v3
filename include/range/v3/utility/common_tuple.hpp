/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_COMMON_TUPLE_HPP
#define RANGES_V3_UTILITY_COMMON_TUPLE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename ...Us, typename Tup, std::size_t...Is>
            std::tuple<Us...> to_std_tuple(Tup && tup, index_sequence<Is...>)
            {
                return std::tuple<Us...>{std::get<Is>(std::forward<Tup>(tup))...};
            }
        }
        /// \endcond

        template<typename ...Ts>
        struct common_tuple
          : std::tuple<Ts...>
        {
        private:
            template<typename That, std::size_t...Is>
            common_tuple(That && that, index_sequence<Is...>)
              : std::tuple<Ts...>{std::get<Is>(std::forward<That>(that))...}
            {}
            std::tuple<Ts...> const & base() const noexcept
            {
                return *this;
            }
        public:
            CONCEPT_REQUIRES(meta::and_c<(bool) DefaultConstructible<Ts>()...>::value)
            common_tuple()
                noexcept(meta::and_c<std::is_nothrow_default_constructible<Ts>::value...>::value)
              : std::tuple<Ts...>{}
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &&>()...>::value)>
            explicit common_tuple(Us &&... us)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &&>::value...>::value)
              : std::tuple<Ts...>{std::forward<Us>(us)...}
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &>()...>::value)>
            common_tuple(std::tuple<Us...> &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &>::value...>::value)
              : common_tuple(that, make_index_sequence<sizeof...(Ts)>{})
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us const &>()...>::value)>
            common_tuple(std::tuple<Us...> const &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us const &>::value...>::value)
              : common_tuple(that, make_index_sequence<sizeof...(Ts)>{})
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &&>()...>::value)>
            common_tuple(std::tuple<Us...> &&that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &&>::value...>::value)
              : common_tuple(std::move(that), make_index_sequence<sizeof...(Ts)>{})
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us const &&>()...>::value)>
            common_tuple(std::tuple<Us...> const &&that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us const &&>::value...>::value)
              : common_tuple(std::move(that), make_index_sequence<sizeof...(Ts)>{})
            {}
            using std::tuple<Ts...>::operator=;
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts &>()...>::value)>
            operator std::tuple<Us...> () &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts &>::value...>::value)
            {
	            return detail::to_std_tuple<Us...>(*this, make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts const &>()...>::value)>
            operator std::tuple<Us...> () const &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts const &>::value...>::value)
            {
	            return detail::to_std_tuple<Us...>(*this, make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts &&>()...>::value)>
            operator std::tuple<Us...> () &&
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts &&>::value...>::value)
            {
	            return detail::to_std_tuple<Us...>(std::move(*this), make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts const &&>()...>::value)>
            operator std::tuple<Us...> () const &&
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts const &&>::value...>::value)
            {
	            return detail::to_std_tuple<Us...>(std::move(*this), make_index_sequence<sizeof...(Ts)>{});
            }
#define LOGICAL_OP(OP, CONCEPT)\
            CONCEPT_REQUIRES(meta::and_c<(bool) CONCEPT<Ts>()...>::value)\
            friend bool operator OP(common_tuple const &a, common_tuple const &b)\
            {\
                return a.base() OP b.base();\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(common_tuple const &a, common_tuple<Us...> const &b)\
            {\
                return a.base() OP b.base();\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(common_tuple const &a, std::tuple<Us...> const &b)\
            {\
                return a.base() OP b;\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(std::tuple<Us...> const &a, common_tuple const &b)\
            {\
                return a OP b.base();\
            }\
            /**/
            LOGICAL_OP(==, EqualityComparable)
            LOGICAL_OP(!=, EqualityComparable)
            LOGICAL_OP(<, TotallyOrdered)
            LOGICAL_OP(<=, TotallyOrdered)
            LOGICAL_OP(>, TotallyOrdered)
            LOGICAL_OP(>=, TotallyOrdered)
#undef LOGICAL_OP
        };

        template<typename F, typename S>
        struct common_pair
          : std::pair<F, S>
        {
        private:
            std::pair<F, S> const & base() const noexcept
            {
                return *this;
            }
        public:
            CONCEPT_REQUIRES(DefaultConstructible<F>() && DefaultConstructible<S>())
            common_pair()
                noexcept(std::is_nothrow_default_constructible<F>::value &&
                    std::is_nothrow_default_constructible<S>::value)
              : std::pair<F, S>{}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &&>() && Constructible<S, S2 &&>())>
            common_pair(F2 &&f2, S2 &&s2)
                noexcept(std::is_nothrow_constructible<F, F2 &&>::value &&
                    std::is_nothrow_constructible<S, S2 &&>::value)
              : std::pair<F, S>{std::forward<F2>(f2), std::forward<S2>(s2)}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &>() && Constructible<S, S2 &>())>
            common_pair(std::pair<F2, S2> &that)
                noexcept(std::is_nothrow_constructible<F, F2 &>::value &&
                    std::is_nothrow_constructible<S, S2 &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 const &>() && Constructible<S, S2 const &>())>
            common_pair(std::pair<F2, S2> const &that)
                noexcept(std::is_nothrow_constructible<F, F2 const &>::value &&
                    std::is_nothrow_constructible<S, S2 const &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &&>() && Constructible<S, S2 &&>())>
            common_pair(std::pair<F2, S2> &&that)
                noexcept(std::is_nothrow_constructible<F, F2 &&>::value &&
                    std::is_nothrow_constructible<S, S2 &&>::value)
              : std::pair<F, S>{std::move(that).first, std::move(that).second}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 const &&>() && Constructible<S, S2 const &&>())>
            common_pair(std::pair<F2, S2> const &&that)
                noexcept(std::is_nothrow_constructible<F, F2 const &&>::value &&
                    std::is_nothrow_constructible<S, S2 const &&>::value)
              : std::pair<F, S>{std::move(that).first, std::move(that).second}
            {}
            using std::pair<F, S>::operator=;
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F &>() && Constructible<S2, S &>())>
            operator std::pair<F2, S2> () &
                noexcept(std::is_nothrow_constructible<F2, F &>::value &&
                    std::is_nothrow_constructible<S2, S &>::value)
            {
                return {this->first, this->second};
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F const &>() && Constructible<S2, S const &>())>
            operator std::pair<F2, S2> () const &
                noexcept(std::is_nothrow_constructible<F2, F const &>::value &&
                    std::is_nothrow_constructible<S2, S const &>::value)
            {
                return {this->first, this->second};
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F &&>() && Constructible<S2, S &&>())>
            operator std::pair<F2, S2> () &&
                noexcept(std::is_nothrow_constructible<F2, F &&>::value &&
                    std::is_nothrow_constructible<S2, S &&>::value)
            {
                return {std::move(*this).first, std::move(*this).second};
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F const &&>() && Constructible<S2, S const &&>())>
            operator std::pair<F2, S2> () const &&
                noexcept(std::is_nothrow_constructible<F2, F const &&>::value &&
                    std::is_nothrow_constructible<S2, S const &&>::value)
            {
                return {std::move(*this).first, std::move(*this).second};
            }
            CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
            friend bool operator ==(common_pair const &a, common_pair const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(common_pair const &a, common_pair<F2, S2> const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(common_pair const &a, std::pair<F2, S2> const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(std::pair<F2, S2> const &a, common_pair const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
            friend bool operator <(common_pair const &a, common_pair const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(common_pair const &a, common_pair<F2, S2> const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(common_pair const &a, std::pair<F2, S2> const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(std::pair<F2, S2> const &a, common_pair const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
#define LOGICAL_OP(OP, CONCEPT, RET)\
            CONCEPT_REQUIRES(CONCEPT<F>() && CONCEPT<S>())\
            friend bool operator OP(common_pair const &a, common_pair const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(common_pair const &a, common_pair<F2, S2> const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(common_pair const &a, std::pair<F2, S2> const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(std::pair<F2, S2> const &a, common_pair const &b)\
            {\
                return RET;\
            }\
            /**/
            LOGICAL_OP(!=, EqualityComparable, !(a == b))
            LOGICAL_OP(<=, TotallyOrdered, !(b < a))
            LOGICAL_OP(>, TotallyOrdered, (b < a))
            LOGICAL_OP(>=, TotallyOrdered, !(a < b))
#undef LOGICAL_OP
        };
    }
}

#endif
