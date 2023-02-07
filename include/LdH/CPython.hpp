#ifndef LdH_CPYTHON_HPP
# define LdH_CPYTHON_HPP

# include <Python.h>

# include "int.hpp"

# if PY_VERSION_HEX >= 0x03'07'00'0'0
#  define LdH_CPython3_7(...) __VA_ARGS__
# else
#  define LdH_CPython3_7(...)
# endif

# if PY_VERSION_HEX >= 0x03'09'00'0'0
#  define LdH_CPython3_9(...) __VA_ARGS__
# else
#  define LdH_CPython3_9(...)
# endif


namespace LdH::CPython
{
    template<
            decltype(PyMethodDef::ml_flags) flags,
            bool varargs, bool keywords, bool one_arg, bool no_args,
            LdH_CPython3_7(bool fastcall,) LdH_CPython3_9(bool fastcall_method,)
            bool class_method, bool static_method
    >
    struct _PyCF1
    {
        static_assert(!(varargs && one_arg), "METH_VARARGS | METH_O not allowed");
        static_assert(!(varargs && no_args), "METH_VARARGS | METH_NOARGS not allowed");
        static_assert(!(keywords && one_arg), "METH_KEYWORDS | METH_O not allowed");
        static_assert(!(keywords && no_args), "METH_KEYWORDS | METH_NOARGS not allowed");
#         if PY_VERSION_HEX < 0x03'07'00'0'0
        static_assert(keywords ? varargs : true, "METH_KEYWORDS requires METH_VARARGS");
#         else
        static_assert(keywords ? varargs | fastcall : true, "METH_KEYWORDS requires METH_VARARGS or METH_FASTCALL");
#         endif
#         if PY_VERSION_HEX < 0x03'07'00'0'0
        static_assert(varargs | one_arg | no_args, "One of METH_NOARGS, METH_O or METH_VARARGS must be specified");
#         else
        static_assert(varargs | one_arg | no_args | fastcall, "One of METH_NOARGS, METH_O, METH_VARARGS or METH_FASTCALL must be specified");
#         endif
#         if PY_VERSION_HEX >= 0x03'07'00'0'0
        static_assert(!(fastcall && varargs), "METH_FASTCALL | METH_VARARGS not allowed");
        static_assert(!(fastcall && one_arg), "METH_FASTCALL | METH_O not allowed");
        static_assert(!(fastcall && no_args), "METH_FASTCALL | METH_NOARGS not allowed");
#         endif
#         if PY_VERSION_HEX >= 0x03'09'00'0'0
        static_assert(fastcall_method ? fastcall : true, "METH_METHOD requires METH_FASTCALL");
        static_assert(fastcall_method ? keywords : true, "METH_METHOD requires METH_KEYWORDS");
#         endif
        static_assert(!(class_method | static_method), "METH_CLASS | METH_STATIC not allowed");
    };

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            true, false, false, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            true, false, false, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            true, true, false, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, PyObject *, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            true, true, false, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, PyObject *, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, PyObject *, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            false, false, true, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject, class arg_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, arg_t *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            false, false, true, false,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class arg_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, arg_t *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject, class arg_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, arg_t *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            false, false, false, true,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            false, false, false, true,
            LdH_CPython3_7(false,) LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

#     if PY_VERSION_HEX >= 0x03'07'00'0'0

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            false, false, false, false,
            true, LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, args_t *const *, Py_ssize_t),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            false, false, false, false,
            true, LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, args_t *const *, Py_ssize_t),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, args_t *const *, Py_ssize_t),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            false, true, false, false,
            true, LdH_CPython3_9(false,)
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class self_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            false, true, false, false,
            true, LdH_CPython3_9(false,)
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

#     if PY_VERSION_HEX >= 0x03'09'00'0'0

    template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
    struct _PyCF1<
            flags,
            false, true, false, false,
            true, true,
            class_method, false
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class defining_class_t = PyTypeObject, class self_t = PyObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(self_t *, defining_class_t *, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF1<
            flags,
            false, true, false, false,
            true, true,
            false, true
    > : public PyMethodDef
    {
        template<class return_t = PyObject, class defining_class_t = PyTypeObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(std::nullptr_t, defining_class_t *, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}

        template<class return_t = PyObject, class defining_class_t = PyTypeObject, class args_t = PyObject>
        _PyCF1(
                char const *name,
                return_t *(*func)(PyObject *, defining_class_t *, args_t *const *, Py_ssize_t, PyObject *),
                char const *doc = nullptr
        ) noexcept: PyMethodDef{
                .ml_name = name,
                .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                .ml_flags = flags,
                .ml_doc = doc
        }
        {}
    };

#     endif
#     endif

    template<decltype(PyMethodDef::ml_flags) flags>
    using _PyCF2 = _PyCF1<
            flags,
            (flags & METH_VARARGS) != 0,
            (flags & METH_KEYWORDS) != 0,
            (flags & METH_O) != 0,
            (flags & METH_NOARGS) != 0,
            LdH_CPython3_7((flags & METH_FASTCALL) != 0,)
            LdH_CPython3_9((flags & METH_METHOD) != 0,)
            (flags & METH_CLASS) != 0,
            (flags & METH_STATIC) != 0
    >;

    template<decltype(PyMethodDef::ml_flags) flags>
    struct _PyCF3 : public _PyCF2<flags>
    {
        using _PyCF2<flags>::_PyCF2;

        operator PyMethodDef() const noexcept
        { return *this; }
    };
}

template<decltype(PyMethodDef::ml_flags) flags>
using PyCF = LdH::CPython::_PyCF3<flags>;

#endif // LdH_CPYTHON_HPP
