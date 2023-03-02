#ifndef LdH_TYPE_TRAITS_HPP
# define LdH_TYPE_TRAITS_HPP

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


}

#endif // LdH_TYPE_TRAITS_HPP
