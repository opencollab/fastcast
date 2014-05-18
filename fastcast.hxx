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
                        constexpr inline static unsigned int pos()
                            {
                                return 0;
                            }
                    };
                };
            };

            struct children
            {
                template<typename U>
                constexpr inline static unsigned int pos()
                    {
                        return 1;
                    }
            };

            constexpr inline static unsigned int number_of_bits()
                {
                    return 0;
                }
        };
    };

    constexpr inline static unsigned int number_of_bits(unsigned int n)
    {
        return n == 0 ? 0 : (1 + number_of_bits(n >> 1));
    }

    template<typename U, typename V>
    constexpr inline static unsigned int corrected_pos()
    {
        return (U::template pos<void>() == 0 ? 0 : (U::template pos<V>() - 1) | (1 << number_of_bits(U::template pos<void>() - 1)));
    }

    template<typename Me>
    constexpr inline static fcast_id_t id()
    {
        return (corrected_pos<typename Me::fcast_hierarchy::parent::fcast_hierarchy::children, Me>() << Me::fcast_hierarchy::number_of_bits()) | id<typename Me::fcast_hierarchy::parent>();
    }

    template<>
    constexpr inline fcast_id_t id<root>() { return 0; }

    template<>
    constexpr inline fcast_id_t id<root::fcast_hierarchy::parent>() { return 0; }

    template<typename U>
    constexpr inline static fcast_id_t base_id()
    {
        return (corrected_pos<typename U::fcast_hierarchy::parent::fcast_hierarchy::children, void>() << U::fcast_hierarchy::number_of_bits()) | id<typename U::fcast_hierarchy::parent>();
    }

    template<unsigned int, typename...>
    struct _children;

    template<unsigned int N, typename U, typename... C>
    struct _children<N, U, C...>
    {
        typedef _children<N + 1, C...> __children__;

        template<typename V>
        constexpr inline static unsigned int pos()
            {
                return std::is_same<U, V>::value ? N : __children__::template pos<V>();
            }
    };

    template<unsigned int N, typename U>
    struct _children<N, U>
    {
        template<typename V>
        constexpr inline static unsigned int pos()
            {
                return N;
            }
    };

    template<typename... C>
    struct children
    {
        typedef _children<1, C...> __children__;

        template<typename V>
        constexpr inline static unsigned int pos()
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

        constexpr inline static unsigned int number_of_bits()
            {
                return fastcast::number_of_bits(base_id<parent>());
            }
    };

    class bad_cast : std::exception
    {
    public:
	
	bad_cast() : std::exception() { }

	virtual const char * what() const noexcept
	    {
		return "Bad cast";
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
                _fcast_id = fastcast::id<V>();
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
                const fcast_id_t x = w->fcast<T, U>::_fcast_id ^ fastcast::id<V>();
                return std::is_base_of<V, W>::value || !x || (fastcast::id<V>() < (x & ((~x) + 1)));
            }

        /**
         * @return true if w is an instance of V
         */
        template<typename V, typename W>
        constexpr inline static bool instanceof(W & w) noexcept
            {
		return instanceof<V, W>(&w);
            }

        /**
         * @return true if the underlying type of w is V
         */
        template<typename V, typename W>
        constexpr inline static bool same(W * w) noexcept
            {
                return fastcast::id<V>() == w->fcast<T, U>::_fcast_id;
            }

        /**
         * @return true if the underlying type of w is V
         */
        template<typename V, typename W>
        constexpr inline static bool same(W & w) noexcept
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
