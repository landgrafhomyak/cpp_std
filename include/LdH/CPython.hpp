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
    namespace Type
    {
        struct Object;
    }

    struct Object : ::PyObject
    {
        ::LdH::CPython::Type::Object *getType() noexcept
        {
#         ifdef Py_TYPE
            return (::LdH::CPython::Type::Object *) (Py_TYPE(this));
#         else
            return (::LdH::CPython::Type::Object *) (::Py_TYPE(this));
#         endif
        }

        void incRef() noexcept
        {
#         ifdef Py_INCREF
            Py_INCREF(this);
#         else
            ::Py_INCREF(this);
#         endif
        }

        void decRef() noexcept
        {
#         ifdef Py_DECREF
            Py_DECREF(this);
#         else
            ::Py_DECREF(this);
#         endif
        }

        [[nodiscard]]
        ::LdH::CPython::Object *newRef()
        {
            this->incRef();
            return this;
        }
    };

    [[noreturn]]
    void Unreachable() noexcept
    {

#     ifdef Py_UNREACHABLE
        Py_UNREACHABLE();
#     else
        ::Py_UNREACHABLE();
#     endif
    }

    namespace Module
    {
#     if PY_VERSION_HEX >= 0x03000000

        struct Definition : public ::PyModuleDef
        {
        public:
            explicit Definition(
                    char const *name,
                    char const *doc = nullptr
            ) noexcept: PyModuleDef{
                    .m_base = PyModuleDef_HEAD_INIT,
                    .m_name = name,
                    .m_doc = doc,
                    .m_size = 0,
                    .m_methods = nullptr,
#                 if PY_VERSION_HEX >= 0x03050000
                    .m_slots = nullptr,
#                 else
                    .m_reload = nullptr,
#                 endif
                    .m_traverse = nullptr,
                    .m_clear = nullptr,
                    .m_free = nullptr
            }
            {}
        };

        [[nodiscard]]
        ::LdH::CPython::Object *Create(::LdH::CPython::Module::Definition *def) noexcept
        {
#         ifdef PyModule_Create
            return (::LdH::CPython::Object *) (PyModule_Create(def));
#         else
            return (::LdH::CPython::Object *) (::PyModule_Create(def));
#         endif
        }

#     endif
    }

    namespace Type
    {
        struct Object : public ::PyTypeObject
        {
        private:
            static void default_dealloc(::PyObject *self)
            {
                ((::LdH::CPython::Object *) self)->getType()->tp_free(self);
            }

        public:
             Object(PyTypeObject const &initializer) noexcept: PyTypeObject(initializer)
            {
                struct _head
                {
                    PyObject_VAR_HEAD
                };

                this->ob_base = _head{PyVarObject_HEAD_INIT(nullptr, 0)}.ob_base;

                if (this->tp_name == nullptr)
                    throw "Type name not specified";

                if (this->tp_dealloc == nullptr)
                    this->tp_dealloc = &default_dealloc;
            }

            enum Read_Result : bool
            {
                Ready_OK = false,
                Ready_TROWN_PY_ERR = true,
            };

            [[nodiscard]]
            Read_Result Ready() noexcept
            {
#             ifdef PyType_Ready
                switch (PyType_Ready(this))
#             else
                switch (::PyType_Ready(this))
#             endif
                {
                    case 0:
                        return Ready_OK;
                    case -1:
                        return Ready_TROWN_PY_ERR;
                    default:
                        static char const fe_message[] = "PyType_Ready returned undocumented value";
#                     ifdef Py_FatalError
                        Py_FatalError(fe_message);
#                     else
                        ::Py_FatalError(fe_message);
#                     endif
                }
            };
        };
    }

    namespace Method
    {

        enum CMethodFlag
        {
            VARARGS = METH_VARARGS,
            KEYWORDS = METH_KEYWORDS,
            LdH_CPython3_7(FASTCALL = METH_FASTCALL,)
            LdH_CPython3_9(METHOD = METH_METHOD,)
            NOARGS = METH_NOARGS,
            ONE_ARG = METH_O,
            CLASS = METH_CLASS,
            STATIC = METH_STATIC,
            COEXIST = METH_COEXIST
        };
    }

    namespace Type
    {
        template<
                decltype(PyMethodDef::ml_flags) flags,
                bool varargs, bool keywords, bool one_arg, bool no_args,
                LdH_CPython3_7(bool fastcall,) LdH_CPython3_9(bool fastcall_method,)
                bool class_method, bool static_method
        >
        struct _func
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
        struct _func<
                flags,
                true, false, false, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject>
             _func(
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
        struct _func<
                flags,
                true, false, false, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject>
             _func(
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
        };

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                true, true, false, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject>
             _func(
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
        struct _func<
                flags,
                true, true, false, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject>
             _func(
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
        };

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                false, false, true, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject, class arg_t = PyObject>
             _func(
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
        struct _func<
                flags,
                false, false, true, false,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class arg_t = PyObject>
             _func(
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
        };

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                false, false, false, true,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject>
             _func(
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
        struct _func<
                flags,
                false, false, false, true,
                LdH_CPython3_7(false,) LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject>
             _func(
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
        };

#     if PY_VERSION_HEX >= 0x03'07'00'0'0

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                false, false, false, false,
                true, LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject, class args_t = PyObject>
             _func(
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
        struct _func<
                flags,
                false, false, false, false,
                true, LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class args_t = PyObject>
             _func(
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
        };

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                false, true, false, false,
                true, LdH_CPython3_9(false,)
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class self_t = PyObject, class args_t = PyObject>
             _func(
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
        struct _func<
                flags,
                false, true, false, false,
                true, LdH_CPython3_9(false,)
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class args_t = PyObject>
             _func(
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
        };

#     if PY_VERSION_HEX >= 0x03'09'00'0'0

        template<decltype(PyMethodDef::ml_flags) flags, bool class_method>
        struct _func<
                flags,
                false, true, false, false,
                true, true,
                class_method, false
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class defining_class_t = PyTypeObject, class self_t = PyObject, class args_t = PyObject>
             _func(
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
        struct _func<
                flags,
                false, true, false, false,
                true, true,
                false, true
        > : public PyMethodDef
        {
            template<class return_t = PyObject, class defining_class_t = PyTypeObject, class args_t = PyObject>
             _func(
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
        };

#     endif
#     endif

        template<decltype(PyMethodDef::ml_flags) flags>
        struct func : public _func<
                flags,
                (flags & Method::VARARGS) != 0,
                (flags & Method::KEYWORDS) != 0,
                (flags & Method::ONE_ARG) != 0,
                (flags & Method::NOARGS) != 0,
                LdH_CPython3_7((flags & Method::FASTCALL) != 0,)
                LdH_CPython3_9((flags & Method::METHOD) != 0,)
                (flags & Method::CLASS) != 0,
                (flags & Method::STATIC) != 0
        >
        {
            using _func<
                    flags,
                    (flags & Method::VARARGS) != 0,
                    (flags & Method::KEYWORDS) != 0,
                    (flags & Method::ONE_ARG) != 0,
                    (flags & Method::NOARGS) != 0,
                    LdH_CPython3_7((flags & Method::FASTCALL) != 0,)
                    LdH_CPython3_9((flags & Method::METHOD) != 0,)
                    (flags & Method::CLASS) != 0,
                    (flags & Method::STATIC) != 0
            >::_func;

             operator PyMethodDef() const noexcept
            {
                return *this;
            }
        };


        template<class ...definitions>
        class RawMethodsTable
        {
        private:
            template<class ...other>
            struct _length
            {
            };

            template<class first, class ...other>
            struct _length<first, other...>
            {
                constexpr static  LdH::size_t COUNT = 1 + _length<other...>::COUNT;
            };

            template<>
            struct _length<>
            {
                constexpr   static  LdH::size_t COUNT = 0;
            };

            PyMethodDef array[_length<definitions...>::COUNT + 1];

        public:
             operator PyMethodDef *() noexcept
            {
                return this->array;
            }

        private:
            template<LdH::size_t pos, class ...>
            struct _init_array
            {
            };

            template<LdH::size_t pos, decltype(PyMethodDef::ml_flags) first_flags, class ...other_t>
            struct _init_array<pos, func<first_flags>, other_t...>
            {
                 static void set(PyMethodDef *start, func<first_flags> first, other_t ...other) noexcept
                {
                    start[pos] = first;
                    _init_array<pos, other_t...>::set(start, other...);
                }
            };

            template<LdH::size_t pos>
            struct _init_array<pos>
            {
                 static void set(PyMethodDef *start) noexcept
                {}
            };

        public:
             RawMethodsTable(definitions... defs) noexcept
            {
                _init_array<0, definitions...>::set(this->array, defs...);
                this->array[_length<definitions...>::COUNT].ml_name = nullptr;
            }
        };
    }

    namespace Method
    {
        struct Definition : public ::PyMethodDef
        {
        public:
            enum class _level
            {
                STATIC,
                CLASS,
                MEMBER
            };
            using _level::STATIC;
            using _level::CLASS;
            using _level::MEMBER;

        private:
            static decltype(PyMethodDef::ml_flags) level2flag(_level level) noexcept
            {
                switch (level)
                {
                    case _level::STATIC:
                        return METH_STATIC;
                    case _level::CLASS:
                        return METH_CLASS;
                    case _level::MEMBER:
                    default:
                        return 0;
                }
            }

        public:
            Definition(
                    char const *name,
                    ::LdH::CPython::Type::Object *(func)(::LdH::CPython::Object *),
                    _level level = MEMBER,
                    char const *doc = nullptr
            ) noexcept: PyMethodDef{
                    .ml_name = name,
                    .ml_meth = (decltype(PyMethodDef::ml_meth)) func,
                    .ml_flags = METH_NOARGS | level2flag(level),
                    .ml_doc = doc
            }
            {}


            template<class name>
            Definition(
                    char const *name,
                    ::LdH::CPython::Type::Object *(func)(::LdH::CPython::Object *, ::LdH::CPython::Object *args),
                    _level level = MEMBER,
                    char const *doc = nullptr
            ) noexcept: PyMethodDef{
                    .ml_name = name,
                    .ml_meth = (decltype(::PyMethodDef::ml_meth)) func,
                    .ml_flags = METH_VARARGS | level2flag(level),
                    .ml_doc = doc
            }
            {}

            Definition(
                    char const *name,
                    ::LdH::CPython::Type::Object *(func)(::LdH::CPython::Object *, ::LdH::CPython::Object *args, ::LdH::CPython::Object *kwargs),
                    _level level = MEMBER,
                    char const *doc = nullptr
            ) noexcept: PyMethodDef{
                    .ml_name = name,
                    .ml_meth = (decltype(::PyMethodDef::ml_meth)) func,
                    .ml_flags = METH_VARARGS | METH_KEYWORDS | level2flag(level),
                    .ml_doc = doc
            }
            {}

            Definition(::std::nullptr_t) noexcept: PyMethodDef{
                    .ml_name = nullptr,
                    .ml_meth = nullptr,
                    .ml_flags = 0,
                    .ml_doc = nullptr
            }
            {}
        };
    }
}

template<decltype(PyMethodDef::ml_flags) flags>
using pycf = LdH::CPython::Type::func<flags>;

#endif // LdH_CPYTHON_HPP
