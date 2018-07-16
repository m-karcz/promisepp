#include <type_traits>
#include <utility>
#include <stdexcept>

namespace promisepp
{
namespace helpers
{

struct badVariantAccess : std::logic_error
{
    using std::logic_error::logic_error;
};

template<typename...Args>
struct IsOneOf;

template<typename T, typename U, typename... Others>
struct IsOneOf<T, U, Others...>
{
    constexpr static bool value = IsOneOf<T, Others...>::value;
};

template<typename T, typename... Others>
struct IsOneOf<T, T, Others...>
{
    constexpr static bool value = true;
};

template<typename T, typename U>
struct IsOneOf<T,U>
{
    constexpr static bool value = false;
};

template<typename T>
struct IsOneOf<T,T>
{
    constexpr static bool value = true;
};

template<typename...Args>
struct IndexOf;

template<typename T, typename... Others>
struct IndexOf<T, T, Others...>
{
    constexpr static std::size_t value = 0;
};

template<typename T, typename U>
struct IndexOf<T,U>;

template<typename T>
struct IndexOf<T,T>
{
    constexpr static std::size_t value = 0;
};

template<typename T, typename U, typename... Others>
struct IndexOf<T, U, Others...>
{
    constexpr static std::size_t value = 1 + IndexOf<T, Others...>::value;
};


typedef void (*voidFunc)(void*);
typedef void (*voidFunc2)(void*, void*);
typedef void (*voidFunc1const1)(void*, const void*);

template<typename...Types>
struct ClassFuncs
{
    template<typename T>
    struct Helper
    {
        static void destructor(void* ptr)
        {
            reinterpret_cast<T*>(ptr)->~T();	
        }
        static void copyAssignment(void* to, const void* from)
        {
            *reinterpret_cast<T*>(to) = *reinterpret_cast<const T*>(from);
        }
        static void moveAssignment(void* to, void* from)
        {
            *reinterpret_cast<T*>(to) = std::move(*reinterpret_cast<T*>(from));
        }
        static void copyInPlace(void* to, const void* from)
        {
            new (to) T (*reinterpret_cast<const T*>(from));
        }
        static void moveInPlace(void* to, void* from)
        {
            new (to) T (std::move(*reinterpret_cast<T*>(from)));
        }
    };
      
    constexpr static voidFunc destructors[] = {Helper<Types>::destructor...};
    constexpr static voidFunc1const1 copyAssignments[] = {Helper<Types>::copyAssignment...};
    constexpr static voidFunc1const1 copiesInPlace[] = {Helper<Types>::copyInPlace...};
    constexpr static voidFunc2 moveAssignments[] = {Helper<Types>::moveAssignment...};
    constexpr static voidFunc2 movesInPlace[] = {Helper<Types>::moveInPlace...};
};

template<typename...Types>
constexpr voidFunc ClassFuncs<Types...>::destructors[];

template<typename...Types>
constexpr voidFunc1const1 ClassFuncs<Types...>::copyAssignments[];

template<typename...Types>
constexpr voidFunc1const1 ClassFuncs<Types...>::copiesInPlace[];

template<typename...Types>
constexpr voidFunc2 ClassFuncs<Types...>::moveAssignments[];

template<typename...Types>
constexpr voidFunc2 ClassFuncs<Types...>::movesInPlace[];

template<typename...Types>
struct Variant
{
private:
    constexpr static std::size_t npos = -1;
public:
    Variant() = default;

    template<typename T>
    Variant(const T& value)
    {
        static_assert(IsOneOf<typename std::decay<T>::type, Types...>::value, "this type does not exists in variant");
        new (&data) T (value);
        index = IndexOf<T, Types...>::value;
    }

    template<typename T>
    Variant(T&& value, std::enable_if<std::is_rvalue_reference<T>::value, void>* = nullptr)
    {
        static_assert(IsOneOf<typename std::decay<T>::type, Types...>::value, "this type does not exists in variant");
        new (&data) T (std::move(value));
        index = IndexOf<T, Types...>::value;
    }

    Variant(const Variant& other)
    {
        if(&other == this)
        {
            return;
        }
        copyInPlace(other);
    }
    Variant(Variant&& other)
    {
        if(&other == this)
        {
            return;
        }
        moveInPlace(std::move(other));
    }
    ~Variant()
    {
        destroy();
    }
    template<typename T>
    typename std::enable_if<not std::is_same<Variant, typename std::remove_reference<T>::type>::value,Variant&>::type
    operator=(T&& value)
    {
        constexpr auto newIndex = IndexOf<T, Types...>::value;
        if(newIndex != index)
        {
            destroy();
            new (&data) T (value);
            index = newIndex;
        }
        else
        {
            quietAs<T>() = std::forward<T>(value);
        }
    }
    Variant& operator=(const Variant& other)
    {
        if(&other == this)
        {
            return *this;
        }
        if(other.index != index)
        {
            destroy();
            copyInPlace(other);
        }
        else
        {
            classFuncs.copyAssignments[index](&data, &other.data);
        }
        return *this;
    }
    Variant& operator=(Variant&& other)
    {
        if(&other == this)
        {
            return *this;
        }
        if(other.index != index)
        {
            destroy();
            moveInPlace(std::move(other));
        }
        else
        {
            classFuncs.moveAssignments[index](&data, &other.data);
            other.index = npos;
        }
        return *this;
    }
    template<typename T>
    T& as()
    {
        if(not has<T>())
        {
            throw badVariantAccess{"Trying to access wrong type"};
        }
    return quietAs<T>();	
    }
    template<typename T>
    const T& as() const
    {
        if(not has<T>())
        {
            throw badVariantAccess{"Trying to access wrong type"};
        }
    return quietAs<T>();
    }
    template<typename T>
    constexpr bool has() const
    {
        return IndexOf<T, Types...>::value == index;
    }
private:
    typename std::aligned_union<0, Types...>::type data;
    std::size_t index = npos;

    void destroy()
    {
        if(index != npos)
        {
            classFuncs.destructors[index](&data);
            index = npos;
        }
    }
    inline void copyInPlace(const Variant& other)
    {
        classFuncs.copiesInPlace[other.index](&data, &other.data);
        index = other.index;
    }
    inline void moveInPlace(Variant&& other)
    {
        classFuncs.movesInPlace[other.index](&data, &other.data);
        index = other.index;
        other.index = npos;
    }
    template<typename T>
    T& quietAs()
    {
        return reinterpret_cast<T&>(data);
    }
    template<typename T>
    const T& quietAs() const
    {
        return reinterpret_cast<const T&>(data);
    }

    constexpr static ClassFuncs<Types...> classFuncs = {};
};

template<typename...Types>
constexpr ClassFuncs<Types...> Variant<Types...>::classFuncs;


}//namespace helpers
}//namespace promisepp
