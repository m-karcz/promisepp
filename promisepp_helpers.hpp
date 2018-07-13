#include <type_traits>

namespace promisepp
{
namespace helpers
{
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

template<typename T, typename U, typename... Others>
struct IndexOf<T, U, Others...>
{
	constexpr static size_t value = 1 + IndexOf<T, Others...>::value;
};

template<typename T, typename... Others>
struct IndexOf<T, T, Others...>
{
	constexpr static size_t value = 0;
};

template<typename T, typename U>
struct IndexOf<T,U>;

template<typename T>
struct IndexOf<T,T>
{
	constexpr static size_t value = 0;
};

typedef void (*dtorFunc)(void*);

template<typename...Types>
struct Dtors
{
	template<typename T>
	struct SingleDtor
	{
		static void fun(void* ptr)
		{
			reinterpret_cast<T*>(ptr)->~T();	
		}
	};
	constexpr static dtorFunc value[] = {SingleDtor<Types>::fun...};
};

template<typename...Types>
constexpr dtorFunc Dtors<Types...>::value[];


template<typename...Types>
struct Variant
{
private:
	constexpr static size_t NONE = -1;
public:
	Variant()
	{}
	template<typename T>
	Variant(T&& value)
	{
		static_assert(IsOneOf<T, Types...>::value, "this type does not exists in variant");
		new (&data) T (value);
		heldIndex = IndexOf<T, Types...>::value;
	}
	~Variant()
	{
		destroy();
	}
	template<typename T>
	Variant& operator=(T&& value)
	{
		constexpr auto newIndex = IndexOf<T, Types...>::value;
		if(newIndex != heldIndex)
		{
			destroy();
			new (&data) T (value);
			heldIndex = newIndex;
		}
		else
		{
			quietTo<T>() = std::forward<T>(value);
		}
	}
	template<typename T>
	T& to()
	{
		return quietTo<T>();	
	}
private:
	typename std::aligned_union<0, Types...>::type data;
	size_t heldIndex = NONE;
	void destroy()
	{
		if(heldIndex != NONE)
		{
			dtors.value[heldIndex](&data);
			heldIndex = NONE;
		}
	}
	template<typename T>
	T& quietTo()
	{
		reinterpret_cast<T&>(data);
	}
	template<typename T>
	const T& quietTo() const
	{
		reinterpret_cast<const T&>(data);
	}

	constexpr static Dtors<Types...> dtors = {};
};

template<typename...Types>
constexpr Dtors<Types...> Variant<Types...>::dtors;


}//namespace helpers
}//namespace promisepp
