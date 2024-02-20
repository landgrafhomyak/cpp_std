//
// Created by Admin on 05.01.2024.
//

#ifndef LdH_STRUCT_SERIALIZER_HPP
#define LdH_STRUCT_SERIALIZER_HPP

# include <cstdint>
# include <type_traits>
# include "int.hpp"
# include "type_traits.hpp"

namespace LdH::StructSerializer
{
    enum class EntityType
    {
        FIELD,
        PADDING,
        ARRAY,
        SKIP
    };

    enum class SerialType
    {
        U8,
        S8,
        U16,
        S16,
        U32,
        S32,
        U64,
        S64,
        _void,
    };


    /* Builder entity *************************************************************************************************/

    template<class member_ptr_t>
    class _member_ptr_parser
    {
        static_assert(LdH::is_same<member_ptr_t, void> && !LdH::is_same<member_ptr_t, void>, "Passed value is not pointer to struct member");
    };

    template<class runtime_object_t, class field_t>
    class _member_ptr_parser<field_t (runtime_object_t::*)>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = false;
        static constexpr bool is_array = false;
    };

    template<class runtime_object_t, class field_t>
    class _member_ptr_parser<field_t volatile (runtime_object_t::*)>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = false;
        static constexpr bool is_array = false;
    };

    template<class runtime_object_t, class field_t>
    class _member_ptr_parser<field_t const (runtime_object_t::*)>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = true;
        static constexpr bool is_array = false;
    };

    template<class runtime_object_t, class field_t>
    class _member_ptr_parser<field_t const volatile (runtime_object_t::*)>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = true;
        static constexpr bool is_array = false;
    };

    template<class runtime_object_t, class field_t, LdH::size_t size>
    class _member_ptr_parser<field_t (runtime_object_t::*)[size]>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = false;
        static constexpr bool is_array = true;
        static constexpr LdH::size_t SIZE = size;
    };

    template<class runtime_object_t, class field_t, LdH::size_t size>
    class _member_ptr_parser<field_t volatile (runtime_object_t::*)[size]>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = false;
        static constexpr bool is_array = true;
        static constexpr LdH::size_t SIZE = size;
    };

    template<class runtime_object_t, class field_t, LdH::size_t size>
    class _member_ptr_parser<field_t const (runtime_object_t::*)[size]>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = true;
        static constexpr bool is_array = true;
        static constexpr LdH::size_t SIZE = size;
    };

    template<class runtime_object_t, class field_t, LdH::size_t size>
    class _member_ptr_parser<field_t const volatile (runtime_object_t::*)[size]>
    {
    public:
        using RUNTIME_OBJECT_T = runtime_object_t;
        using FIELD_T = field_t;
        static constexpr bool is_const = true;
        static constexpr bool is_array = true;
        static constexpr LdH::size_t SIZE = size;
    };


    template<class member_ptr_t, bool array_expected>
    class _member_ptr_asserter
    {
        static_assert(array_expected != false && array_expected != true, "Your compiler is broken, bool have more then 2 states");
    };

    template<class member_ptr_t>
    class _member_ptr_asserter<member_ptr_t, false>
    {
    private:
        using _parsed = _member_ptr_parser<member_ptr_t>;
    public:
        static_assert(!_parsed::is_array, "Field must BE NOT array");
        static_assert(!_parsed::is_const, "Field must be non-const");
        using RUNTIME_OBJECT_T = typename _parsed::RUNTIME_OBJECT_T;
        using FIELD_T = typename _parsed::FIELD_T;
    };

    template<class member_ptr_t>
    class _member_ptr_asserter<member_ptr_t, true>
    {
    private:
        using _parsed = _member_ptr_parser<member_ptr_t>;
    public:
        static_assert(_parsed::is_array, "Field must BE array");
        static_assert(!_parsed::is_const, "Field must be non-const");
        using RUNTIME_OBJECT_T = typename _parsed::RUNTIME_OBJECT_T;
        using FIELD_T = typename _parsed::FIELD_T;
        static constexpr LdH::size_t SIZE = _parsed::SIZE;
    };

    template<EntityType entity_type, SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
    class Entity
    {
    };

    template<SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
    class Entity<EntityType::FIELD, serial_type, size, runtime_object_t, member_ptr>
    {
        static_assert(size == 0, "Field size must be 0 because is not an array");

    private:
        explicit constexpr Entity() noexcept = default;

        template<SerialType st, auto mp>
        friend
        constexpr Entity<EntityType::FIELD, st, 0, typename _member_ptr_asserter<decltype(mp), false>::RUNTIME_OBJECT_T, mp> field() noexcept;
    };

    class _void_object
    {
    private:
        int void_field;

        template<EntityType entity_type, SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
        friend
        class Entity;

        template<LdH::size_t s>
        friend
        constexpr Entity<EntityType::PADDING, SerialType::_void, s, _void_object, &_void_object::void_field> padding() noexcept;

        template<LdH::size_t s>
        friend
        constexpr Entity<EntityType::SKIP, SerialType::_void, s, _void_object, &_void_object::void_field> skip() noexcept;
    };

    template<SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
    class Entity<EntityType::PADDING, serial_type, size, runtime_object_t, member_ptr>
    {
        static_assert(LdH::is_same<runtime_object_t, void>, "Padding's runtime_object type must be void");
        static_assert(serial_type == SerialType::_void, "Padding's serial type must be _void");
    private:
        explicit constexpr Entity() noexcept = default;


        template<LdH::size_t s>
        friend
        constexpr Entity<EntityType::PADDING, SerialType::_void, s, void, &_void_object::void_field> padding() noexcept;
    };

    template<SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
    class Entity<EntityType::SKIP, serial_type, size, runtime_object_t, member_ptr>
    {
        static_assert(LdH::is_same<runtime_object_t, void>, "Padding's runtime_object type must be void");
        static_assert(serial_type == SerialType::_void, "Padding's serial type must be _void");
    private:
        explicit constexpr Entity() noexcept = default;


        template<LdH::size_t s>
        friend
        constexpr Entity<EntityType::SKIP, SerialType::_void, s, void, &_void_object::void_field> padding() noexcept;
    };

    template<SerialType serial_type, LdH::size_t size, class runtime_object_t, auto member_ptr>
    class Entity<EntityType::ARRAY, serial_type, size, runtime_object_t, member_ptr>
    {
    private:
        explicit constexpr Entity() noexcept = default;

        template<SerialType t, LdH::size_t s, auto mp>
        friend
        constexpr Entity<EntityType::ARRAY, t, s, typename _member_ptr_asserter<decltype(mp), true>::RUNTIME_OBJECT_T, mp> array() noexcept;
    };


    template<SerialType serial_type, auto member_ptr>
    constexpr Entity<EntityType::FIELD, serial_type, 0, typename _member_ptr_asserter<decltype(member_ptr), false>::RUNTIME_OBJECT_T, member_ptr> field() noexcept
    { return Entity<EntityType::FIELD, serial_type, 0, typename _member_ptr_asserter<decltype(member_ptr), false>::RUNTIME_OBJECT_T, member_ptr>{}; }


    template<LdH::size_t size>
    constexpr Entity<EntityType::PADDING, SerialType::_void, size, void, &_void_object::void_field> padding() noexcept
    { return Entity<EntityType::PADDING, SerialType::_void, size, void, &_void_object::void_field>{}; }


    template<LdH::size_t size>
    constexpr Entity<EntityType::SKIP, SerialType::_void, size, void, &_void_object::void_field> skip() noexcept
    { return Entity<EntityType::SKIP, SerialType::_void, size, void, &_void_object::void_field>{}; }


    template<SerialType serial_type, LdH::size_t size, auto member_pointer>
    constexpr Entity<EntityType::ARRAY, serial_type, size, typename _member_ptr_asserter<decltype(member_pointer), true>::RUNTIME_OBJECT_T, member_pointer> array() noexcept
    {
        static_assert(size == _member_ptr_asserter<decltype(member_pointer), true>::SIZE, "Requested array size differs from actual field size");
        return Entity<EntityType::ARRAY, serial_type, size, typename _member_ptr_asserter<decltype(member_pointer), true>::RUNTIME_OBJECT_T, member_pointer>{};
    }


    /* Primitives serializers *****************************************************************************************/

    template<LdH::size_t byte_size>
    class _uint_serializer
    {
        static_assert(byte_size != 0, "Number with size 0 not need to be serialized");
        static_assert(byte_size <= sizeof(std::conditional_t<(sizeof(LdH::u64f) > sizeof(std::uintmax_t)), LdH::u64f, std::uintmax_t>), "Integer type too big to fit into C++ primitive integer type");
    public:
        using UINT_T = std::conditional_t<byte_size == 1, LdH::u8f, std::conditional_t<byte_size == 2, LdH::u16f, std::conditional_t<byte_size <= 4, LdH::u32f, std::conditional_t<byte_size <= 8, LdH::u64f, std::uintmax_t>>>>;
    private:
        template<LdH::size_t pos, class buffer_t>
        static void _serialize(buffer_t buffer, UINT_T value, LdH::size_t offset) noexcept
        {

            buffer[offset + byte_size - pos - 1] = (unsigned char) (value & 0xFFu);
            if constexpr (pos > 0)
            {
                _serialize<pos - 1, buffer_t>(buffer, value >> 8u, offset);
            }
        }

    public:
        template<class buffer_t>
        static inline void serialize(buffer_t buffer, UINT_T value, LdH::size_t offset) noexcept
        {
            _serialize<byte_size - 1, buffer_t>(buffer, value, offset);
        }

    private:
        template<LdH::size_t pos, class buffer_t>
        static UINT_T _deserialize(buffer_t buffer, LdH::size_t offset) noexcept
        {
            UINT_T value = (((UINT_T) (buffer[offset + pos])) & 0xFFu);
            if constexpr (pos > 0)
                value = (value << 8) | _deserialize<pos - 1, buffer_t>(buffer, offset);
            return value;
        }

    public:
        template<class buffer_t>
        static inline UINT_T deserialize(buffer_t buffer, LdH::size_t offset) noexcept
        {
            return _deserialize<byte_size - 1, buffer_t>(buffer, offset);
        }
    };

    template<SerialType t>
    class _primitive_serializer
    {
        static_assert(t != SerialType::_void, "SerialType::_void is internal type, mustn't be used for serialization");
    };

    template<>
    class _primitive_serializer<SerialType::U8>
    {
    public:
        static constexpr LdH::size_t SIZE = 1;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<1>::serialize<buffer_t>(buffer, (uint_fast8_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (uint_fast8_t) _uint_serializer<1>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::S8>
    {
    public:
        static constexpr LdH::size_t SIZE = 1;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<1>::serialize<buffer_t>(buffer, (uint_fast8_t) (int_fast8_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (int_fast8_t) (uint_fast8_t) _uint_serializer<1>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::U16>
    {
    public:
        static constexpr LdH::size_t SIZE = 2;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<2>::serialize<buffer_t>(buffer, (uint_fast16_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (uint_fast16_t) _uint_serializer<2>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::S16>
    {
    public:
        static constexpr LdH::size_t SIZE = 2;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<2>::serialize<buffer_t>(buffer, (uint_fast16_t) (int_fast16_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t const *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (int_fast16_t) (uint_fast16_t) _uint_serializer<2>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::U32>
    {
    public:
        static constexpr LdH::size_t SIZE = 4;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<4>::serialize<buffer_t>(buffer, (uint_fast32_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (uint_fast32_t) _uint_serializer<4>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::S32>
    {
    public:
        static constexpr LdH::size_t SIZE = 4;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<4>::serialize<buffer_t>(buffer, (uint_fast32_t) (int_fast32_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (int_fast32_t) (uint_fast32_t) _uint_serializer<4>::deserialize<buffer_t>(buffer, offset);
        }
    };


    template<>
    class _primitive_serializer<SerialType::U64>
    {
    public:
        static constexpr LdH::size_t SIZE = 8;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<8>::serialize<buffer_t>(buffer, (uint_fast64_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (uint_fast64_t) _uint_serializer<8>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<>
    class _primitive_serializer<SerialType::S64>
    {
    public:
        static constexpr LdH::size_t SIZE = 8;

        template<class field_t, class buffer_t>
        static inline void serialize(field_t value, buffer_t buffer, LdH::size_t offset) noexcept
        {
            _uint_serializer<8>::serialize<buffer_t>(buffer, (uint_fast64_t) (int_fast64_t) value, offset);
        }

        template<class field_t, class buffer_t>
        static inline void deserialize(field_t *dst, const buffer_t buffer, LdH::size_t offset) noexcept
        {
            *dst = (int_fast64_t) (uint_fast64_t) _uint_serializer<8>::deserialize<buffer_t>(buffer, offset);
        }
    };

    template<SerialType serial_type>
    class _array_serializer
    {
    public:
        template<LdH::size_t size, class field_t, class buffer_t>
        static inline void serialize(field_t const *array, buffer_t buffer, LdH::size_t offset) noexcept
        {
            if constexpr (size > 0)
            {
                serialize<size - 1, field_t, buffer_t>(array, buffer, offset);
                _primitive_serializer<serial_type>::template serialize<field_t, buffer_t>(array[size - 1], buffer, offset + (size - 1) * _primitive_serializer<serial_type>::SIZE);
            }
        }

        template<LdH::size_t size, class field_t, class buffer_t>
        static inline void deserialize(field_t *array, buffer_t buffer, LdH::size_t offset) noexcept
        {
            if constexpr (size > 0)
            {
                deserialize<size - 1, field_t, buffer_t>(array, buffer, offset);
                _primitive_serializer<serial_type>::template deserialize<field_t, buffer_t>(&(array[size - 1]), buffer, offset + (size - 1) * _primitive_serializer<serial_type>::SIZE);
            }
        }
    };

    template<LdH::size_t size, class buffer_t>
    static inline void _fill0(buffer_t buffer, LdH::size_t offset) noexcept
    {
        if constexpr (size > 0)
        {
            buffer[offset + (size - 1)] = (unsigned char) 0;
            _fill0<size - 1, buffer_t>(buffer, offset);
        }
    }


    /* Struct object implementation ***********************************************************************************/

    template<class runtime_object_t, class ...descriptors_t>
    class _struct_impl_wrapper;

    template<class runtime_object_t, class ...descriptors_t>
    class _struct_impl
    {
    };

    template<class runtime_object_t>
    class _struct_impl<runtime_object_t>
    {
    public:
        _struct_impl() noexcept = delete;

        template<class buffer_t>
        static void serialize(runtime_object_t const *, buffer_t, LdH::size_t = 0) noexcept
        {}

        template<class buffer_t>
        static void deserialize(runtime_object_t *, const buffer_t, LdH::size_t = 0) noexcept
        {}

    private:
        template<class, class ...>
        friend
        class _struct_impl_wrapper;

        template<class, class ...>
        friend
        class _struct_impl;

        static constexpr LdH::size_t SIZE = 0;
    };


    template<class runtime_object_t, SerialType serial_type, class ...descriptors_t, auto member>
    class _struct_impl<runtime_object_t, Entity<EntityType::FIELD, serial_type, 0, runtime_object_t, member>, descriptors_t...>
    {
    public:
        _struct_impl() noexcept = delete;

    private:
        using _next = _struct_impl<runtime_object_t, descriptors_t...>;

        template<class buffer_t>
        static void serialize(runtime_object_t const *object, buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _primitive_serializer<serial_type>::template serialize<typename _member_ptr_asserter<decltype(member), false>::FIELD_T, buffer_t>(object->*(member), buffer, offset);
            _next::template serialize<buffer_t>(object, buffer, offset + _primitive_serializer<serial_type>::SIZE);
        }

        template<class buffer_t>
        static void deserialize(runtime_object_t *object, const buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _primitive_serializer<serial_type>::template deserialize<typename _member_ptr_asserter<decltype(member), false>::FIELD_T, buffer_t>(&(object->*(member)), buffer, offset);
            _next::template deserialize<buffer_t>(object, buffer, offset + _primitive_serializer<serial_type>::SIZE);
        }

        template<class, class ...>
        friend
        class _struct_impl_wrapper;

        template<class, class ...>
        friend
        class _struct_impl;

        static constexpr LdH::size_t SIZE = _next::SIZE + _primitive_serializer<serial_type>::SIZE;
    };

    template<class runtime_object_t, SerialType serial_type, LdH::size_t size, class ...descriptors_t, auto member>
    class _struct_impl<runtime_object_t, Entity<EntityType::ARRAY, serial_type, size, runtime_object_t, member>, descriptors_t...>
    {
    public:
        _struct_impl() noexcept = delete;

    private:
        using _next = _struct_impl<runtime_object_t, descriptors_t...>;

        template<class buffer_t>
        static void serialize(runtime_object_t const *object, buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _array_serializer<serial_type>::template serialize<size, typename _member_ptr_asserter<decltype(member), true>::FIELD_T, buffer_t>(object->*(member), buffer, offset);
            _next::template serialize<buffer_t>(object, buffer, offset + size * _primitive_serializer<serial_type>::SIZE);
        }

        template<class buffer_t>
        static void deserialize(runtime_object_t *object, const buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _array_serializer<serial_type>::template deserialize<size, typename _member_ptr_asserter<decltype(member), true>::FIELD_T, buffer_t>(object->*(member), buffer, offset);
            _next::template deserialize<buffer_t>(object, buffer, offset + size * _primitive_serializer<serial_type>::SIZE);
        }

        template<class, class ...>
        friend
        class _struct_impl_wrapper;

        template<class, class ...>
        friend
        class _struct_impl;

        static constexpr LdH::size_t SIZE = _next::SIZE + _primitive_serializer<serial_type>::SIZE * size;
    };


    template<class runtime_object_t, LdH::size_t size, class ...descriptors_t, auto member>
    class _struct_impl<runtime_object_t, Entity<EntityType::PADDING, SerialType::_void, size, runtime_object_t, member>, descriptors_t...>
    {
    public:
        _struct_impl() noexcept = delete;

    private:
        using _next = _struct_impl<runtime_object_t, descriptors_t...>;

        template<class buffer_t>
        static void serialize(runtime_object_t const *object, buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _fill0<size, buffer_t>(buffer, offset);
            _next::template serialize<buffer_t>(object, buffer, offset + size);
        }

        template<class buffer_t>
        static void deserialize(runtime_object_t *object, const buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _next::template deserialize<buffer_t>(object, buffer, offset + size);
        }

        template<class, class ...>
        friend
        class _struct_impl_wrapper;

        template<class, class ...>
        friend
        class _struct_impl;

        static constexpr LdH::size_t SIZE = _next::SIZE + size;
    };

    template<class runtime_object_t, LdH::size_t size, class ...descriptors_t, auto member>
    class _struct_impl<runtime_object_t, Entity<EntityType::SKIP, SerialType::_void, size, runtime_object_t, member>, descriptors_t...>
    {
    public:
        _struct_impl() noexcept = delete;

    private:
        using _next = _struct_impl<runtime_object_t, descriptors_t...>;

        template<class buffer_t>
        static void serialize(runtime_object_t const *object, buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _next::template serialize<buffer_t>(object, buffer, offset + size);
        }

        template<class buffer_t>
        static void deserialize(runtime_object_t *object, const buffer_t buffer, LdH::size_t offset = 0) noexcept
        {
            _next::template deserialize<buffer_t>(object, buffer, offset + size);
        }

        template<class, class ...>
        friend
        class _struct_impl_wrapper;

        template<class, class ...>
        friend
        class _struct_impl;

        static constexpr LdH::size_t SIZE = _next::SIZE + size;
    };

    template<class runtime_object_t, class ...descriptors_t>
    constexpr _struct_impl_wrapper<runtime_object_t, descriptors_t...> Struct(descriptors_t... descriptors) noexcept;

    template<class runtime_object_t, class ...descriptors_t>
    class _struct_impl_wrapper
    {
    private:
        using _impl = _struct_impl<runtime_object_t, descriptors_t...>;

        explicit constexpr _struct_impl_wrapper() noexcept = default;

        template<class ro, class ...ds>
        friend
        constexpr _struct_impl_wrapper<ro, ds...> Struct(ds... descriptors) noexcept;

    public:
        template<class buffer_t>
        void serialize(runtime_object_t const *object, buffer_t buffer, LdH::size_t offset = 0) const noexcept
        {
            _impl::template serialize<buffer_t>(object, buffer, offset);
        }

        template<class buffer_t>
        void deserialize(runtime_object_t *object, const buffer_t buffer, LdH::size_t offset = 0) const noexcept
        {
            _impl::template deserialize<buffer_t>(object, buffer, offset);
        }

        const LdH::size_t size = _impl::SIZE;
    };

    template<class runtime_object_t, class ...descriptors_t>
    constexpr _struct_impl_wrapper<runtime_object_t, descriptors_t...> Struct(descriptors_t...) noexcept
    {
        return _struct_impl_wrapper<runtime_object_t, descriptors_t...>();
    }

}


#endif // LdH_STRUCT_SERIALIZER_HPP
