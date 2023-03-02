#ifndef LdH_PACK_HPP
# define LdH_PACK_HPP

#include "type_traits.hpp"
#include "int.hpp"

namespace LdH
{
    enum PackOrder
    {
        PackOrder_LittleEndian = '<',
        PackOrder_BigEndian = '>',
    };

    template<::LdH::size_t size>
    class pack_padding
    {
    public:
        pack_padding() noexcept = default;
    };

    template<::LdH::u8 value>
    class pack_const
    {
    public:
        pack_const() noexcept = default;
    };

    template<typename item_type, LdH::size_t size>
    using pack_array = item_type[size];

    template<typename T>
    constexpr bool supports_pack =
            ::LdH::is_same<T, s8> || ::LdH::is_same<T, u8> ||
            ::LdH::is_same<T, s16> || ::LdH::is_same<T, u16> ||
            ::LdH::is_same<T, s32> || ::LdH::is_same<T, u32> ||
            ::LdH::is_same<T, s64> || ::LdH::is_same<T, u64>;

    template<::LdH::size_t size>
    constexpr bool supports_pack<pack_padding<size>> = true;

    template<::LdH::size_t size>
    pack_padding<size> pack_padding_v{};

    template<::LdH::u8 value>
    constexpr bool supports_pack<pack_const<value>> = true;

    template<::LdH::u8 value>
    pack_const<value> pack_const_v{};

    namespace __private
    {
        template<PackOrder ORDER, typename u_value_t, class buffer_t, ::LdH::size_t size, size_t offset = 0>
        struct pack_integer
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept;

            static u_value_t unpack(buffer_t buffer) noexcept;
        };

        template<typename u_value_t, class buffer_t, ::LdH::size_t size, ::LdH::size_t offset>
        struct pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((::LdH::u8) (value >> (offset * 8))) & 0xFF;
                if constexpr(offset + 1 < size)
                    ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << (offset * 8);
                if constexpr(offset + 1 < size)
                    value |= ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };

        template<typename u_value_t, class buffer_t, ::LdH::size_t size, ::LdH::size_t offset>
        struct pack_integer<PackOrder_LittleEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((::LdH::u8) (value >> ((size - offset - 1) * 8))) & 0xFF;
                if constexpr(offset < size)
                    ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << ((size - offset - 1) * 8);
                if constexpr(offset < size)
                    value |= ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };
    }

    template<PackOrder ORDER, typename T, class buffer_t>
    struct _pack1
    {
        static_assert(::LdH::supports_pack<T>, "Requested type can't be packed or unpacked");
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s8, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static buffer_t pack(buffer_t buffer, ::LdH::s8 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) (::LdH::u8) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::s8 *storage) noexcept
        {
            *storage = (::LdH::s8) (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u8, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static buffer_t pack(buffer_t buffer, ::LdH::u8 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::u8 *storage) noexcept
        {
            *storage = (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s16, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 2;

        static buffer_t pack(buffer_t buffer, short value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) (::LdH::u16) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::s16 *storage) noexcept
        {
            *storage = (::LdH::s16) (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u16, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 2;

        static buffer_t pack(buffer_t buffer, ::LdH::u16 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::u16 *storage) noexcept
        {
            *storage = (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s32, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 4;

        static buffer_t pack(buffer_t buffer, ::LdH::s32 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) (::LdH::u32) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::s32 *storage) noexcept
        {
            *storage = (::LdH::s32) (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u32, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 4;

        static buffer_t pack(buffer_t buffer, ::LdH::u32 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::u32 *storage) noexcept
        {
            *storage = (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s64, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 8;

        static buffer_t pack(buffer_t buffer, ::LdH::s64 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) (::LdH::u64) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::s64 *storage) noexcept
        {
            *storage = (::LdH::s64) (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u64, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 8;

        static buffer_t pack(buffer_t buffer, ::LdH::u64 value) noexcept
        {
            ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) value);
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::u64 *storage) noexcept
        {
            *storage = (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer);
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, ::LdH::size_t SIZE, class buffer_t>
    struct _pack1<ORDER, ::LdH::pack_padding<SIZE>, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = SIZE;

        static buffer_t pack(buffer_t buffer, ::LdH::pack_padding<SIZE> value) noexcept
        {
            buffer[0] = '\0';
            if constexpr(SIZE > 0)
                ::LdH::_pack1<ORDER, ::LdH::pack_padding<SIZE - 1>, decltype(buffer + 1)>::pack(buffer + 1, {});
            return buffer + PACKED_SIZE;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::pack_padding<SIZE> *storage) noexcept
        {
            return buffer + PACKED_SIZE;
        }
    };

    template<PackOrder ORDER, ::LdH::u8 VALUE, class buffer_t>
    struct _pack1<ORDER, ::LdH::pack_const<VALUE>, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static buffer_t pack(buffer_t buffer, ::LdH::pack_const<VALUE> value) noexcept
        {
            buffer[0] = VALUE;
            return buffer + 1;
        }

        static buffer_t unpack(buffer_t buffer, ::LdH::pack_const<VALUE> *storage) noexcept
        {
            return buffer + 1;
        }
    };

    template<PackOrder ORDER, typename item_t, LdH::size_t size, class buffer_t>
    struct _pack1<ORDER, item_t[size], buffer_t>
    {
    private:
        static constexpr ::LdH::size_t _NOLOOP_OPTIMIZATION_LIMIT = 64;

    public:
        static const ::LdH::size_t PACKED_SIZE;

        static buffer_t pack(buffer_t buffer, item_t value[size]) noexcept;

        // double pointer required for compatibility with common interface
        static buffer_t unpack(buffer_t buffer, item_t *storage[size]) noexcept;
    };

    template<typename T, class buffer_t = char *, PackOrder ORDER = PackOrder_BigEndian>
    constexpr ::LdH::size_t packed_size = ::LdH::_pack1<ORDER, T, buffer_t>::PACKED_SIZE;

    template<::LdH::size_t size>
    constexpr ::LdH::size_t packed_size<pack_padding<size>> = size;

    template<PackOrder ORDER, typename T, class buffer_t>
    buffer_t pack1(buffer_t buffer, T value) noexcept
    {
        return ::LdH::_pack1<ORDER, T, buffer_t>::pack(buffer, value);
    }

    template<PackOrder ORDER, typename T, class buffer_t>
    buffer_t unpack1(buffer_t buffer, T *storage) noexcept
    {
        return ::LdH::_pack1<ORDER, T, buffer_t>::unpack(buffer, storage);
    }


    template<PackOrder ORDER, typename item_t, LdH::size_t size, class buffer_t>
    constexpr LdH::size_t  _pack1<ORDER, item_t[size], buffer_t>::PACKED_SIZE = packed_size<item_t, buffer_t, ORDER> * size;

    template<PackOrder ORDER, typename item_t, LdH::size_t size, class buffer_t>
    buffer_t _pack1<ORDER, item_t[size], buffer_t>::pack(buffer_t buffer, item_t value[size]) noexcept
    {
        item_t *p = value;
        item_t *end = p + size;
        while (p != end)
            buffer = pack1<ORDER, item_t, buffer_t>(buffer, *p++);
        return buffer;
    }

    template<PackOrder ORDER, typename item_t, LdH::size_t size, class buffer_t>
    buffer_t _pack1<ORDER, item_t[size], buffer_t>::unpack(buffer_t buffer, item_t *storage[size]) noexcept
    {
        item_t *p = *storage;
        item_t *end = p + size;
        while (p != end)
            buffer = unpack1<ORDER, item_t, buffer_t>(buffer, p++);
        return buffer;
    }

    template<PackOrder ORDER, typename A, typename ...T>
    class pack_struct
    {
    public:
        static constexpr ::LdH::size_t PACKED_SIZE = pack_struct<ORDER, T...>::PACKED_SIZE + packed_size<A>;

        template<class buffer_t>
        static buffer_t pack(buffer_t buffer, A first_value, T... other_values) noexcept
        {
            return pack_struct<ORDER, T...>::template pack<buffer_t>(
                    pack1<ORDER, A, buffer_t>(buffer, first_value),
                    other_values...
            );
        }

        template<class buffer_t>
        static buffer_t unpack(buffer_t buffer, A *first_storage, T *... other_storages) noexcept
        {
            return LdH::pack_struct<ORDER, T...>::template unpack<buffer_t>(
                    LdH::unpack1<ORDER, A, buffer_t>(buffer, first_storage),
                    other_storages...
            );
        }
    };

    template<PackOrder ORDER, typename A>
    class pack_struct<ORDER, A>
    {
    public:
        static constexpr LdH::size_t PACKED_SIZE = packed_size<A>;

        template<class buffer_t>
        static buffer_t pack(buffer_t buffer, A value) noexcept
        {
            return LdH::pack1<ORDER, A, buffer_t>(buffer, value);
        }

        template<class buffer_t>
        static buffer_t unpack(buffer_t buffer, A *storage) noexcept
        {
            return LdH::unpack1<ORDER, A, buffer_t>(buffer, storage);
        }
    };
}

#endif // LdH_PACK_HPP
