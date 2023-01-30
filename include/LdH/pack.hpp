#ifndef LdH_PACK_HPP
# define LdH_PACK_HPP

#include <cstdint>
#include <type_traits>

namespace LdH
{

    namespace __private
    {
        using pack_deault_buffer_t = std::uint_least8_t *;
    }

    enum PackOrder
    {
        PackOrder_LittleEndian = '<',
        PackOrder_BigEndian = '>',
    };


    template<typename T>
    constexpr bool supports_pack =
            std::is_same_v<T, char> || std::is_same_v<T, unsigned char> ||
            std::is_same_v<T, short> || std::is_same_v<T, unsigned short> ||
            std::is_same_v<T, long> || std::is_same_v<T, unsigned long> ||
            std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>;

    namespace __private
    {
        template<PackOrder ORDER, typename u_value_t, class buffer_t, std::size_t size, std::size_t offset = 0>
        struct pack_integer
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept;

            static u_value_t unpack(buffer_t buffer) noexcept;
        };

        template<typename u_value_t, class buffer_t, std::size_t size, std::size_t offset>
        struct pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((uint_least8_t) (value >> (offset * 8))) & 0xFF;
                if constexpr(offset < size)
                    pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << (offset * 8);
                if constexpr(offset < size)
                    value |= pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };

        template<typename u_value_t, class buffer_t, std::size_t size, std::size_t offset>
        struct pack_integer<PackOrder_LittleEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((uint_least8_t) (value >> ((size - offset - 1) * 8))) & 0xFF;
                if constexpr(offset < size)
                    pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << ((size - offset - 1) * 8);
                if constexpr(offset < size)
                    value |= pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };

        template<PackOrder ORDER, typename T, class buffer_t>
        struct pack1
        {
            static_assert(supports_pack<T>, "Requested type can't be packed or unpacked");
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, char, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 1;

            static void pack(buffer_t buffer, char value) noexcept
            { pack_integer<ORDER, unsigned char, buffer_t, PACKED_SIZE>::pack(buffer, (unsigned char) value); }

            static char unpack(buffer_t buffer) noexcept
            { return (char) pack_integer<ORDER, unsigned char, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, unsigned char, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 1;

            static void pack(buffer_t buffer, unsigned char value) noexcept
            { pack_integer<ORDER, unsigned char, buffer_t, PACKED_SIZE>::pack(buffer, value); }

            static unsigned char unpack(buffer_t buffer) noexcept
            { return pack_integer<ORDER, unsigned char, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, short, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 2;

            static void pack(buffer_t buffer, short value) noexcept
            { pack_integer<ORDER, unsigned short, buffer_t, PACKED_SIZE>::pack(buffer, (unsigned short) value); }

            static short unpack(buffer_t buffer) noexcept
            { return (short) pack_integer<ORDER, unsigned short, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, unsigned short, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 2;

            static void pack(buffer_t buffer, unsigned short value) noexcept
            { pack_integer<ORDER, unsigned short, buffer_t, PACKED_SIZE>::pack(buffer, value); }

            static unsigned short unpack(buffer_t buffer) noexcept
            { return pack_integer<ORDER, unsigned short, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, long, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 4;

            static void pack(buffer_t buffer, long value) noexcept
            { pack_integer<ORDER, unsigned long, buffer_t, PACKED_SIZE>::pack(buffer, (unsigned long) value); }

            static long unpack(buffer_t buffer) noexcept
            { return (long) pack_integer<ORDER, unsigned long, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, unsigned long, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 4;

            static void pack(buffer_t buffer, unsigned long value) noexcept
            { pack_integer<ORDER, unsigned long, buffer_t, PACKED_SIZE>::pack(buffer, value); }

            static unsigned long unpack(buffer_t buffer) noexcept
            { return pack_integer<ORDER, unsigned long, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, long long, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 8;

            static void pack(buffer_t buffer, long long value) noexcept
            { pack_integer<ORDER, unsigned long long, buffer_t, PACKED_SIZE>::pack(buffer, (unsigned long long) value); }

            static long long unpack(buffer_t buffer) noexcept
            { return (long long) pack_integer<ORDER, unsigned long long, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, unsigned long long, buffer_t>
        {
            static constexpr std::size_t PACKED_SIZE = 8;

            static void pack(buffer_t buffer, unsigned long long value) noexcept
            { pack_integer<ORDER, unsigned long long, buffer_t, PACKED_SIZE>::pack(buffer, value); }

            static unsigned long long unpack(buffer_t buffer) noexcept
            { return pack_integer<ORDER, unsigned long long, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };
    }

    template<typename T>
    constexpr std::size_t packed_size = __private::pack1<PackOrder_BigEndian, T, uint_least8_t *>::SIZE;

    template<PackOrder ORDER, typename T, class buffer_t = __private::pack_deault_buffer_t>
    void pack1(buffer_t buffer, T value) noexcept
    {
        __private::pack1<ORDER, T, buffer_t>::pack(buffer, value);
    }

    template<PackOrder ORDER, typename T, class buffer_t = __private::pack_deault_buffer_t>
    T unpack1(buffer_t buffer) noexcept
    {
        return __private::pack1<ORDER, T, buffer_t>::unpack(buffer);
    }

    namespace __private
    {
        template<typename A, typename ...T>
        struct packed_struct_size
        {
            static constexpr std::size_t SIZE = packed_size<A> + packed_struct_size<T...>::SIZE;
        };

        template<typename T>
        struct packed_struct_size<T>
        {
            static constexpr std::size_t SIZE = packed_size<T>;
        };

    }

    template<PackOrder ORDER, typename A, typename ...T>
    class pack_meta
    {
    public:
        static constexpr std::size_t PACKED_SIZE = __private::packed_struct_size<A, T...>::SIZE;

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void pack(void *buffer, A first_value, T... other_values) noexcept
        {
            __private::pack1<ORDER, A, buffer_t>::pack(first_value, buffer);
            pack_meta<ORDER, T...>::template pack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_values...);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(void *buffer, A *first_storage, T *... other_storages) noexcept
        {
            *first_storage = __private::pack1<ORDER, A, buffer_t>::unpack(buffer);
            pack_meta<ORDER, T...>::template unpack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_storages...);
        }
    };

    template<PackOrder ORDER, typename A>
    class pack_meta<ORDER, A>
    {
    public:
        static constexpr std::size_t PACKED_SIZE = packed_size<A>;

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void pack(A value, void *buffer) noexcept
        {
            __private::pack1<ORDER, A, buffer_t>::pack(value, buffer);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(A *storage, void *buffer) noexcept
        {
            *storage = __private::pack1<ORDER, A, buffer_t>::unpack(buffer);
        }
    };

}

#endif // LdH_PACK_HPP
