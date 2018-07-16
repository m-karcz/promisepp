#include "promisepp.hpp"
#include <gtest/gtest.h>

TEST(context, shoudlBeNeitherRejectedNorResolvedIfValueNotSet)
{
	using namespace promisepp;
	std::promise<helpers::Variant<ResolvedValue<int>, RejectedValue<int>>> pr;
	promisepp::Context<int,int> ctx{pr.get_future().share()};

	ASSERT_FALSE(ctx.isResolved());
	ASSERT_FALSE(ctx.isRejected());
}

TEST(context, shouldBeResolvedAfterSettingResolvedValue)
{
	using namespace promisepp;
	std::promise<helpers::Variant<ResolvedValue<int>, RejectedValue<int>>> pr;
	promisepp::Context<int,int> ctx{pr.get_future().share()};

	ASSERT_FALSE(ctx.isResolved());
	ASSERT_FALSE(ctx.isRejected());

	pr.set_value({ResolvedValue<int>{5}});

	ASSERT_TRUE(ctx.isResolved());
	ASSERT_FALSE(ctx.isRejected());
}

TEST(context, shouldBeRejectedAfterSettingRejectvedValue)
{
	using namespace promisepp;
	std::promise<helpers::Variant<ResolvedValue<int>, RejectedValue<int>>> pr;
	promisepp::Context<int,int> ctx{pr.get_future().share()};

	ASSERT_FALSE(ctx.isResolved());
	ASSERT_FALSE(ctx.isRejected());

	pr.set_value({RejectedValue<int>{5}});

	ASSERT_FALSE(ctx.isResolved());
	ASSERT_TRUE(ctx.isRejected());
}
