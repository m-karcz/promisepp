#include "promisepp_variant.hpp"
#include <gtest/gtest.h>
#include <vector>

inline void ASSERT_TRUE_T(bool value) //due to problems with template argument in ASSERT_TRUE
{
    ASSERT_TRUE(value);
}

inline void ASSERT_FALSE_T(bool value) //as above
{
    ASSERT_FALSE(value);
};

template<typename T1, typename T2>
inline void ASSERT_EQ_T(const T1& a, const T2& b) //as above
{
    ASSERT_EQ(a, b);
}

TEST(indexOf, shouldReturnZero)
{
    using namespace promisepp::helpers;
    auto single = IndexOf<bool, bool>::value;
    ASSERT_EQ(single, 0);
}

TEST(indexOf, shouldReturnOtherThanZero)
{
    using namespace promisepp::helpers;
    auto shouldBe2 = IndexOf<int,   std::string,
                                    bool,
                                    int
                                        >::value;

    auto shouldBe5 = IndexOf<std::string,   int,
                                            bool,
                                            char,
                                            std::vector<int>,
                                            std::vector<char>,
                                            std::string
                                                    >::value;

    ASSERT_EQ_T(shouldBe2, 2);
    ASSERT_EQ_T(shouldBe5, 5);
}

TEST(isOneOf, shouldReturnTrue)
{
    using namespace promisepp::helpers;
    ASSERT_TRUE_T(IsOneOf<int, int>::value);
    ASSERT_TRUE_T(IsOneOf<int, char, bool, int, std::string>::value);
    ASSERT_TRUE_T(IsOneOf<int, int, int>::value);
}

TEST(isOneOf, shouldReturnFalse)
{
    using namespace promisepp::helpers;
    ASSERT_FALSE_T(IsOneOf<bool, char>::value);
    ASSERT_FALSE_T(IsOneOf<int, std::string, char, void>::value);
}

TEST(variant, copiesShouldHaveOtherValueAfterChangingOne)
{
    using namespace promisepp::helpers;

    constexpr int BEFORE_CHANGE = 1;
    constexpr int AFTER_CHANGE = 5;

    Variant<int, std::string> first{BEFORE_CHANGE};

    Variant<int, std::string> second;

    second = first;

    first.as<int>() = AFTER_CHANGE;

    ASSERT_EQ_T(first.as<int>(), AFTER_CHANGE);
    ASSERT_EQ_T(second.as<int>(), BEFORE_CHANGE);
}

TEST(variant, afterMoveCtorShouldNotContainAnyType)
{
    using namespace promisepp::helpers;

    Variant<int, char> first{5};

    Variant<int, char> second = std::move(first);

    ASSERT_FALSE_T(first.has<int>());
    ASSERT_FALSE_T(first.has<char>());
}

TEST(variant, afterMoveAssignmentShouldNotContainAnyType)
{
    using namespace promisepp::helpers;

    Variant<int, bool> first{true};

    Variant<int, bool> second;

    second = std::move(first);

    ASSERT_FALSE_T(first.has<int>());
    ASSERT_FALSE_T(first.has<bool>());
}
