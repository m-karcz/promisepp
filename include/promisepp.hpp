#include "promisepp_variant.hpp"
#include <future>

namespace promisepp
{

template<typename T>
struct ResolvedValue
{
	T value;
};

template<typename T>
struct RejectedValue
{
	T value;
};

template<typename ResolvedType, typename RejectedType>
struct Context
{
	using ResolvedTrait = ResolvedValue<ResolvedType>;
	using RejectedTrait = RejectedValue<RejectedType>;

	Context(std::shared_future<helpers::Variant<ResolvedTrait, RejectedTrait>> fut) : fut(fut)
	{}
	inline bool isReady() const
	{
		using namespace std::chrono;
		return fut.wait_for(seconds(0)) == std::future_status::ready;
	}
	inline void getReady()
	{
		fut.wait();	
	}
	inline bool isResolved() const
	{
		return isReady() and fut.get().template has<ResolvedTrait>();
	}
	inline bool isRejected() const
	{
		return isReady() and fut.get().template has<RejectedTrait>();
	}
	inline ResolvedType& getResolved() const
	{
		return fut.get().template as<ResolvedTrait>().value;
	}
	inline RejectedType& getRejected() const
	{
		return fut.get().template as<RejectedTrait>().value;
	}
private:
	std::shared_future<helpers::Variant<ResolvedTrait, RejectedTrait>> fut;
};
}//namespace promisepp
