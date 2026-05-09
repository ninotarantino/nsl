#include <array>
#include <complex>
#include <exception>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nsl/math/polynomial.hh>

namespace {

inline constexpr double tolerance = 1e-10;

TEST(Polynomial, Evaluate) {
    // 2nd order
    {
        nsl::math::polynomial poly(4.0, -3.0, 2.0);
        EXPECT_NEAR(poly.evaluate(3.0), 29.0, tolerance);
    }

    // 3rd order
    {
        nsl::math::polynomial poly(9.0f, 0.0f, 2.0f, -4.0f);
        EXPECT_NEAR(poly.evaluate(-4.0f), -588.0f, tolerance);
    }
}

TEST(Polynomial, Discriminant) {
    // 2nd order
    {
        nsl::math::polynomial poly(4.0, -3.0, 2.0);
        EXPECT_NEAR(nsl::math::discriminant(poly), -23.0, tolerance);
    }

    // 3rd order
    {
        nsl::math::polynomial poly(9.0f, 0.0f, 2.0f, -4.0f);
        EXPECT_NEAR(nsl::math::discriminant(poly), -35280.0f, tolerance);
    }
}

TEST(Polynomial, Roots) {
    // Complex roots
    {
        nsl::math::polynomial poly(1.0, 0.0, 9.0);
        EXPECT_THROW(nsl::math::real_roots(poly), std::runtime_error);

        auto roots = nsl::math::complex_roots(poly);
        EXPECT_NEAR(std::get<0>(roots).real(), 0.0, tolerance);
        EXPECT_NEAR(std::get<0>(roots).imag(), -3.0, tolerance);
        EXPECT_NEAR(std::get<1>(roots).real(), 0.0, tolerance);
        EXPECT_NEAR(std::get<1>(roots).imag(), 3.0, tolerance);
    }

    // Real roots
    {
        nsl::math::polynomial poly(1.0, -5.0, 6.0);
        auto roots = nsl::math::real_roots(poly);
        EXPECT_NEAR(std::get<0>(roots), 2.0, tolerance);
        EXPECT_NEAR(std::get<1>(roots), 3.0, tolerance);
    }
}

TEST(Polynomial, TransformCoefficients) {
    using testing::DoubleNear;
    using testing::Pointwise;

    nsl::math::polynomial poly(1.0, 2.0, 3.0);
    poly.transform_coefficients([](double& coeff) {coeff *= -2.0;});
    const std::array expected {-2.0, -4.0, -6.0};
    EXPECT_THAT(poly.coeffs, Pointwise(DoubleNear(tolerance), expected));
}

TEST(Polynomial, ReassignCoefficients) {
    using testing::Eq;
    using testing::Pointwise;

    nsl::math::polynomial poly(1.0, 2.0, 3.0);
    std::array new_coeffs {5.0, -10.0, 1.0};
    poly = new_coeffs;
    EXPECT_THAT(poly.coeffs, Pointwise(Eq(), new_coeffs));
}

}
