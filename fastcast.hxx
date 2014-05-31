// Copyright (c) 2014, Calixte DENIZET
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef __FASTCAST_HXX__
#define __FASTCAST_HXX__ 1

#if __cplusplus < 201103L
# error "Fastcast must be used with C++11"
#else

#include <cinttypes>
#include <exception>
#include <type_traits>

/**
 * The basic idea is to find a single uint to identify a class and to be able to easily determinate if a class
 * B derived from A.
 * For example, if we have a class A which have children B, C, D, B have children E and F, and D have a child G:
 * A--B--E, F
 *    |
 *    C
 *    |
 *    D--G
 *
 * A will have id 1 (because it is the root).
 * A has 3 children: B(0), C(1), D(2).
 * So the id of:
 *  - B will be 1001 (1 00 1: first 1 is a delimiter, 00 is the position and 1 is the id of A);
 *  - C will be 1011 (1 01 1: first 1 is a delimiter, 01 is the position and 1 is the id of A);
 *  - D will be 1101 (1 10 1: first 1 is a delimiter, 10 is the position and 1 is the id of A).
 * In following the same scheme, the id of:
 *  - E will be 101001 (1 0 1001: first 1 is a delimiter, 0 is the position and 1001 is the id of B);
 *  - F will be 111001 (1 1 1001: first 1 is a delimiter, 1 is the position and 1001 is the id of B);
 *  - G will be 101101 (1 0 1101: first 1 is a delimiter, 0 is the position and 1101 is the id of D).
 * In this case only a uint8_t is required to store the id.
 *
 * So now it is easy to check if a class is deriving from another one: just check if the derived id "finished"
 * its (supposed) parent's one.
 * For example, E is a child of B, because B's id finished E's id but G is not a child of B because G's id didn't
 * finish B's one.
 *
 */

namespace fastcast
{
    // The type used for fcast_id
    typedef uint64_t fcast_id_t;

    struct root
    {
        struct fcast_hierarchy
        {
            struct parent
            {
                struct fcast_hierarchy
                {
                    struct children
                    {
                        template<typename U>
                        constexpr static unsigned int pos() noexcept
                            {
                                return 0;
                            }
                    };
                };
            };

            struct children
            {
                template<typename U>
                constexpr static unsigned int pos() noexcept
                    {
                        return 1;
                    }
            };

            template<typename T>
            constexpr static unsigned int number_of_bits() noexcept
                {
                    return 0;
                }
        };
    };

    // Define parent struct
    template<typename...>
    struct parents;

    template<typename U, typename...P>
    struct parents<U, P...>
    {
        typedef U type;
        typedef parents<P...> __parents__;

        // it is choosen when U derives from V
        template<typename V>
        static typename std::decay<decltype(static_cast<U&&>(true ? std::declval<U>() : std::declval<V>()))>::type _fcast_test_(int);

        // else...
        template<typename V>
        static typename std::decay<decltype(__parents__::template _fcast_test_<V>(0))>::type _fcast_test_(char);
    };

    // Terminal
    template<typename U>
    struct parents<U>
    {
        typedef U type;

        template<typename V>
        static U _fcast_test_(int);
    };

    // U is something like parent<...> and P is a base class for one of the parent's args
    // type will be the corresponding arg
    template<typename P, typename U>
    struct _get_parent
    {
        // it is choosen when V has a function _fcast_test_, i.e. when V is a parent struct
        template<typename V>
        static typename std::decay<decltype(V::template _fcast_test_<P>(0))>::type _fcast_test_(int);

        // else...
        template<typename V>
        static V _fcast_test_(char);

        typedef typename std::decay<decltype(_fcast_test_<U>(0))>::type type;
    };

    /**
     * @return the number of bits used to represent the argument
     */
    constexpr unsigned int number_of_bits(unsigned int n) noexcept
    {
        return n == 0 ? 0 : (1 + number_of_bits(n >> 1));
    }

    /**
     * @return the corrected position of a child in children list
     */
    template<typename U, typename V>
    constexpr unsigned int corrected_pos() noexcept
    {
        return (U::template pos<void>() == 0 ? 0 : (U::template pos<V>() - 1) | (1 << number_of_bits(U::template pos<void>() - 1)));
    }

    // The id of a child according to its position in the list of the parent's children
    // and parent's id
    template<typename T, typename Me>
    struct _fcast_id_
    {
        constexpr static fcast_id_t id = (corrected_pos<typename _get_parent<T, typename Me::fcast_hierarchy::parent>::type::fcast_hierarchy::children, Me>() << Me::fcast_hierarchy::template number_of_bits<T>()) | _fcast_id_<T, typename _get_parent<T, typename Me::fcast_hierarchy::parent>::type>::id;
    };

    // Partial specialization of the template function _fcast_id_<T, Me>
    template<typename T>
    struct _fcast_id_<T, root>
    {
        constexpr static fcast_id_t id = 0;
    };

    // Partial specialization of the template function _fcast_id_<T, Me>
    template<typename T>
    struct _fcast_id_<T, root::fcast_hierarchy::parent>
    {
        constexpr static fcast_id_t id = 0;
    };

    /**
     * @return the id of the parent
     */
    template<typename T, typename U>
    constexpr fcast_id_t base_id() noexcept
    {
        return (corrected_pos<typename _get_parent<T, typename U::fcast_hierarchy::parent>::type::fcast_hierarchy::children, void>() << U::fcast_hierarchy::template number_of_bits<T>()) | _fcast_id_<T, typename _get_parent<T, typename U::fcast_hierarchy::parent>::type>::id;
    }

    // Define _children struct
    template<unsigned int, typename...>
    struct _children;

    // Define recursively the chidlren using a variadic template
    template<unsigned int N, typename U, typename... C>
    struct _children<N, U, C...>
    {
        typedef _children<N + 1, C...> __children__;

        /**
         * @return the position of a child in children list
         */
        template<typename V>
        constexpr static unsigned int pos() noexcept
            {
                return std::is_same<U, V>::value ? N : __children__::template pos<V>();
            }
    };

    // Last template used in the recursion
    template<unsigned int N, typename U>
    struct _children<N, U>
    {
        /**
         * @return the position of a child in children list
         */
        template<typename V>
        constexpr static unsigned int pos() noexcept
            {
                return N;
            }
    };

    // Define children struct used in the hierarchy definition
    template<typename... C>
    struct children
    {
        typedef _children<1, C...> __children__;

        /**
         * @return the position of a child in children list
         */
        template<typename V>
        constexpr static unsigned int pos() noexcept
            {
                return __children__::template pos<V>();
            }
    };

    // Define the hierarchy
    template<typename Parent, typename Children = void>
    struct hierarchy
    {
        typedef Parent parent;
        typedef Children children;

        template<typename T>
        constexpr static unsigned int number_of_bits() noexcept
            {
                return fastcast::number_of_bits(base_id<T, typename _get_parent<T, parent>::type>());
            }
    };

    // bad_cast exception is used when instanceof as a reference as parameter
    class bad_cast : std::exception
    {
    public:

        bad_cast() : std::exception() { }

        virtual const char * what() const noexcept
            {
                return "Bad fast cast";
            }
    };

    // To be derivated to have a fcast_id
    template<typename T, typename U>
    struct fcast
    {
        U _fcast_id;

        /**
         * Set the _fcast_id field
         */
        template<typename V>
        inline void set_id() noexcept
            {
                _fcast_id = fastcast::_fcast_id_<fcast<T, U>, V>::id;
            }

        /**
         * @return true if w is an instance of V
         */
        template<typename V, typename W>
        inline static bool instanceof(W * w) noexcept
            {
                // Check if V::fcast_id ended w->fcast::_fcast_id in binary representation
                // For example, if a=1011011 and b=1011 then b is ending a.
                // In the previous case x=a^b=1010000 and x&-x=10000
                constexpr U _id_ = fastcast::_fcast_id_<fcast<T, U>, V>::id;
                const fcast_id_t x = w->fcast<T, U>::_fcast_id ^ _id_;

                return std::is_base_of<V, W>::value || !x || (_id_ < (x & ((~x) + 1)));
            }

        /**
         * @return true if w is an instance of V
         */
        template<typename V, typename W>
        inline static bool instanceof(W & w) noexcept
            {
                return instanceof<V, W>(&w);
            }

        /**
         * @return true if the underlying type of w is V
         */
        template<typename V, typename W>
        inline static bool same(W * w) noexcept
            {
                return fastcast::_fcast_id_<fcast<T, U>, V>::id == w->fcast<T, U>::_fcast_id;
            }

        /**
         * @return true if the underlying type of w is V
         */
        template<typename V, typename W>
        inline static bool same(W & w) noexcept
            {
                return same<V, W>(&w);
            }

        /**
         * Cast w to a V* pointer
         * @return the casted pointer or nullptr if V is not an instance of U
         */
        template<typename V, typename W>
        inline static V * cast(W * w) noexcept
            {
                return instanceof<V>(w) ? static_cast<V *>(w) : nullptr;
            }

        /**
         * Just an alias for static_cast
         */
        template<typename V, typename W>
        inline static V & cast_unchecked(W * w)
            {
                return static_cast<V *>(w);
            }

        /**
         * Cast w to a V reference
         * @return the casted reference or throw a fastcast::bad_cast exception
         */
        template<typename V, typename W>
        inline static V & cast(W & w)
            {
                return instanceof<V>(w) ? static_cast<V &>(w) : throw fastcast::bad_cast();
            }

        /**
         * Just an alias for static_cast
         */
        template<typename V, typename W>
        inline static V & cast_unchecked(W & w)
            {
                return static_cast<V &>(w);
            }
    };

} // namespace fastcast

#endif // __cplusplus < 201103L
#endif // __FASTCAST_HXX__
