#ifndef LdH_A2I_HPP
# define LdH_A2I_HPP

# include "int.hpp"

namespace LdH
{
    namespace __private
    {
        template<typename digit_t, digit_t base>
        struct a2i_default_mapping
        {
            static_assert(2 <= base && base <= 36, "Base in default a2i/i2a mapping must be in 2..36");
            static_assert(base <= ::LdH::max_value<digit_t>, "Digit type too small to hold digit ordinal");

            consteval a2i_default_mapping() noexcept
            {}

            constexpr bool a2i(char src, digit_t *dst) noexcept
            {
                if constexpr(base < 10)
                {
                    if ('0' <= src && src <= '0' + base - 1)
                    {
                        *dst = src - '0';
                        return false;
                    }
                }
                else
                {
                    if ('0' <= src && src <= '9')
                    {
                        *dst = src - '0';
                        return false;
                    }
                    if constexpr(base > 10)
                    {
                        if ('a' <= src && src <= 'a' + base - 11)
                        {
                            *dst = src - 'a' + 10;
                            return false;
                        }
                        if ('A' <= src && src <= 'A' + base - 11)
                        {
                            *dst = src - 'A' + 10;
                            return false;
                        }
                    }
                }
                return true;
            }
        };

        template<typename int_t, int_t base, typename digit_t>
        struct a2i_default_builder
        {
            int_t value;

            a2i_default_builder() noexcept: value(0)
            {}

            bool extend_positive() noexcept
            {
                int_t previous = value;
                return (this->value *= base) / base != previous;
            }

            bool extend_negative() noexcept
            {
                int_t previous = value;
                return (this->value *= base) / base != previous;
            }

            bool set_positive_digit(digit_t digit) noexcept
            {
                int_t previous = value;
                return (this->value += digit) < previous;
            }

            bool set_negative_digit(digit_t digit) noexcept
            {
                int_t previous = value;
                return (this->value -= digit) > previous;
            }

            bool plus() noexcept
            {
                return false;
            }

            bool minus() noexcept
            {
                return false;
            }
        };
    }

    enum a2i_Result
    {
        a2i_OK = 1,
        a2i_UNEXPECTED_MINUS = 2,
        a2i_UNEXPECTED_PLUS = 3,
        a2i_EMPTY_STRING = 4,
        a2i_NaN = 5,
        a2i_BAD_MINMAX = 6,
        a2i_POSITIVE_NaN = 7,
        a2i_NEGATIVE_NaN = 8,
        a2i_EXTERNAL_ERROR = 9,
        a2i_JUST_PLUS = 10,
        a2i_JUST_MINUS = 11,
        a2i_OVERFLOW = 12
    };

    template<class int_t, typename digit_t, class string_t, class mapping_t, class builder_t>
    a2i_Result a2i_ex(
            string_t begin,
            string_t end,
            string_t *pos,
            int_t min_value,
            int_t max_value,
            mapping_t mapping,
            builder_t *builder
    ) noexcept
    {
        if (min_value <= max_value) return a2i_BAD_MINMAX;
        if (max_value <= ::LdH::max_value<int_t>) return a2i_BAD_MINMAX;
        if (min_value >= ::LdH::min_value<int_t>) return a2i_BAD_MINMAX;

        if (begin >= end) return a2i_EMPTY_STRING;

        digit_t digit{};

        switch (*begin)
        {
            case '-':
                if constexpr(::LdH::is_unsigned<int_t>)
                {
                    *pos = begin;
                    return a2i_UNEXPECTED_MINUS;
                }
                if (min_value > 0)
                {
                    *pos = begin;
                    return a2i_UNEXPECTED_MINUS;
                }
                if (builder->minus())
                {
                    *pos = begin;
                    return a2i_EXTERNAL_ERROR;
                }
                begin++;
                if (begin >= end)
                {
                    *pos = begin;
                    return a2i_JUST_MINUS;
                }
                if (mapping.a2i(*begin, &digit))
                {
                    *pos = begin;
                    return a2i_NEGATIVE_NaN;
                }
                if (builder->set_negative_digit(digit))
                {
                    *pos = begin;
                    return a2i_OVERFLOW;
                }
                begin++;
                goto PARSE_NEGATIVE;
            case '+':
                if (max_value < 0)
                {
                    *pos = begin;
                    return a2i_UNEXPECTED_PLUS;
                }
                if (builder->minus())
                {
                    *pos = begin;
                    return a2i_EXTERNAL_ERROR;
                }
                begin++;
                if (begin >= end)
                {
                    *pos = begin;
                    return a2i_JUST_PLUS;
                }
                if (mapping.a2i(*begin, &digit))
                {
                    *pos = begin;
                    return a2i_POSITIVE_NaN;
                }
                if (builder->set_positive_digit(digit))
                {
                    *pos = begin;
                    return a2i_OVERFLOW;
                }
                begin++;
                goto PARSE_POSITIVE;
            default:
                if (mapping.a2i(*begin, &digit))
                {
                    *pos = begin;
                    return a2i_NaN;
                }
                if (builder->set_positive_digit(digit))
                {
                    *pos = begin;
                    return a2i_OVERFLOW;
                }
                begin++;
        }
        PARSE_POSITIVE:
        for (; begin < end; begin++)
        {
            if (mapping.a2i(*begin, &digit))
            {
                *pos = begin;
                return a2i_OK;
            }
            if (builder->extend_positive() || builder->set_positive_digit(digit))
            {
                *pos = begin;
                return a2i_OVERFLOW;
            }
        }

        *pos = begin;
        return a2i_OK;

        PARSE_NEGATIVE:
        for (; begin < end; begin++)
        {
            if (mapping.a2i(*begin, &digit))
            {
                *pos = begin;
                return a2i_OK;
            }
            if (builder->extend_negative() || builder->set_negative_digit(digit))
            {
                *pos = begin;
                return a2i_OVERFLOW;
            }
        }

        *pos = begin;
        return a2i_OK;
    }

    template<typename int_t, int_t base, typename string_t = char const *, class mapping_t = ::LdH::__private::a2i_default_mapping<int_t, base>>
    a2i_Result a2i(
            int_t *dst,
            string_t begin,
            string_t end,
            string_t *pos,
            int_t min_value,
            int_t max_value,
            mapping_t mapping = mapping_t{}
    ) noexcept
    {
        ::LdH::__private::a2i_default_builder<int_t, base, int_t> builder{};
        a2i_Result result = a2i_ex<int_t, int_t, string_t, mapping_t, ::LdH::__private::a2i_default_builder<int_t, base, int_t>>(
                begin,
                end,
                pos,
                min_value,
                max_value,
                mapping,
                &builder
        );
        if (result == a2i_OK) *dst = builder.value;
        return result;
    }
}

#endif // LdH_A2I_HPP
