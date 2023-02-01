#ifndef LdH_CCHECK_HPP
# define LdH_CCHECK_HPP

namespace LdH
{
    namespace __private
    {
        template<typename A, typename B>
        struct is_same
        {
            static constexpr bool VALUE = false;
        };

        template<typename A>
        struct is_same<A, A>
        {
            static constexpr bool VALUE = true;
        };
    }

    template<typename A, typename B>
    constexpr bool is_same = ::LdH::__private::is_same<A, B>::VALUE;
}

#endif // LdH_CCHECK_HPP
