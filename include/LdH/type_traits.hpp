#ifndef LdH_TYPE_TRAITS_HPP
# define LdH_TYPE_TRAITS_HPP

# include <type_traits>

namespace LdH
{
    template<typename A, typename B>
    constexpr bool is_same = std::is_same_v<A, B>;

    template<typename T, typename first_variant, typename ...variants>
    constexpr bool is_one_of = is_same<T, first_variant> || is_one_of<T, variants...>;

    template<typename T, typename last_variant>
    constexpr bool is_one_of<T, last_variant> = is_same<T, last_variant>;

    template<typename F>
    struct fun_info
    {
    };

    template<typename R>
    struct fun_info<R (*)()>
    {
    public:
        using return_type = R;
    };

    template<typename R, typename T>
    struct fun_info<R (T::*)()>
    {
    public:
        using return_type = R;
        using this_type = R;
    };

    template<typename R, typename A1>
    struct fun_info<R (*)(A1)>
    {
    public:
        using return_type = R;
        using arg1 = A1;
    };

    template<typename R, typename T, typename A1>
    struct fun_info<R (T::*)(A1)>
    {
    public:
        using return_type = R;
        using this_type = R;
        using arg1 = A1;
    };

    namespace __private
    {
        template<typename T>
        struct ref_helper
        {
        public:
            using no_ref = T;
            static constexpr bool is_lvalue_reference = false;
            static constexpr bool is_rvalue_reference = false;
        };

        template<typename T>
        struct ref_helper<T &>
        {
        public:
            using no_ref = T;
            static constexpr bool is_lvalue_reference = true;
            static constexpr bool is_rvalue_reference = false;
        };

        template<typename T>
        struct ref_helper<T &&>
        {
        public:
            using no_ref = T;
            static constexpr bool is_lvalue_reference = false;
            static constexpr bool is_rvalue_reference = true;
        };
    }

    template<typename T>
    using clear_reference = typename __private::ref_helper<T>::no_ref;

    template<typename T>
    constexpr bool is_reference = __private::ref_helper<T>::is_lvalue_reference || __private::ref_helper<T>::is_rvalue_reference;

    template<typename T>
    constexpr bool is_lvalue_reference = __private::ref_helper<T>::is_lvalue_reference;

    template<typename T>
    constexpr bool is_rvalue_reference = __private::ref_helper<T>::is_rvalue_reference;

    template<class T>
    constexpr bool is_enum = std::is_enum<T>::value;

    template<class T>
    constexpr bool is_union = std::is_union<T>::value;

    using std::declval;


}

#endif // LdH_TYPE_TRAITS_HPP
