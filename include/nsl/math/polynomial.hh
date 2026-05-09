#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>

namespace nsl::math {

/**
 * @brief Represents a polynomial.
 * @tparam Degree The degree of the polynomial - i.e., the power of the highest exponent.
 * @tparam Datatype The underlying data type of the coefficients.
 *
 * @code{.cpp}
 * // Explicit: 2x^2 - 4
 * polynomial<2, double> quadratic(2.0, 0.0, -4.0);
 *
 * // Template argument deduction: -5x^3 - x^2 + 4x - 2
 * polynomial cubic(-5.0, -1.0, 5.0, -2.0);
 * @endcode
 */
template <std::size_t Degree, typename Datatype = double>
class polynomial {
public:
    static constexpr std::size_t degree = Degree;
    static constexpr std::size_t num_terms = Degree + 1;

    using coefficients = std::array<Datatype, num_terms>;

    constexpr polynomial() noexcept = default;
    template <typename... Args>
    constexpr polynomial(Args&&... args) : coeffs{args...} {
        static_assert(sizeof...(args) == num_terms, "Incorrect number of coefficients provided.");
    }
    template <typename Container>
    constexpr polynomial(const Container& data) noexcept(false) { (*this) = data; }

    /**
     * @brief Evaluate the polynomial at a given x value.
     * @param x The x value at which the polynomial is evaluated.
     * @note Uses Horner's method.
     */
    constexpr Datatype evaluate(const Datatype x) const noexcept {
        Datatype result {coeffs[0]};
        for (std::size_t index = 1; index < coeffs.size(); ++index) {
            result = result * x + coeffs[index];
        }
        return result;
    }

    /**
     * @brief Apply a transformation to each coefficient.
     * @param func Function object to be applied to each coefficient.
     *
     * @code{.cpp}
     * auto addone = [](double& value) {value += 1.0;};
     * polynomial.transform_coefficients(addone);
     * @endcode
     */
    template <typename Functor>
    void transform_coefficients(Functor&& func) {
        std::for_each(coeffs.begin(), coeffs.end(), func);
    }

    constexpr Datatype operator()(const Datatype x) const { return evaluate(x); }

    template <typename Data>
    polynomial& operator=(const Data& data) noexcept(false) {
        if (std::size(data) != num_terms) {
            throw std::runtime_error("Incorrect number of coefficients provided.");
        }
        std::copy(std::begin(data), std::end(data), coeffs.begin());
        return *this;
    }

    coefficients coeffs {}; /* (--) Element 0 is the highest power */
};

template <typename... Args>
polynomial(Args...) -> polynomial<sizeof...(Args) - 1, std::common_type_t<Args...>>;

/**
 * Calculates the discriminant of the quadratic polynomial.
 */
template <typename Datatype>
constexpr Datatype discriminant(const polynomial<2, Datatype>& poly) {
    const auto [a, b, c] = poly.coeffs;
    return (b * b) - (4.0 * a * c);
}

/**
 * Calculates the discriminant of the cubic polynomial.
 */
template <typename Datatype>
constexpr Datatype discriminant(const polynomial<3, Datatype>& polynomial) {
    const auto [a, b, c, d] = polynomial.coeffs;
    const auto b2 = b * b;
    const auto c2 = c * c;
    return (b2 * c2) - (4.0 * a * c2 * c) - (4.0 * b2 * b * d) -
        (27.0 * a * a * d * d) + (18.0 * a * b * c * d);
}

template <typename Datatype>
using complex_polynomial_roots = std::pair<std::complex<Datatype>, std::complex<Datatype>>;

/**
 * @brief Evaluate the roots of the 2nd order polynomial.
 * @retval The two complex roots of the polynomial.
 */
template <typename Datatype>
complex_polynomial_roots<Datatype> complex_roots(const polynomial<2, Datatype>& poly, Datatype y = 0) {
    const std::complex a = poly.coeffs[0];
    const std::complex b = poly.coeffs[1];
    const std::complex c = poly.coeffs[2] - y;
    const std::complex disc = discriminant(poly);
    const std::complex root_term = std::sqrt(disc);
    const std::complex inv_2a = 1.0 / (2.0 * a);
    return {inv_2a * (-b - root_term), inv_2a * (-b + root_term)};
}

template <typename Datatype>
using real_polynomial_roots = std::pair<Datatype, Datatype>;

/**
 * @brief Evaluate the roots of the 2nd order polynomial.
 * @retval The two roots of the polynomial.
 * @notes If the roots are complex, throws an error.
 */
template <typename Datatype>
real_polynomial_roots<Datatype> real_roots(const polynomial<2, Datatype>& poly, Datatype y = 0) {
    const Datatype a = poly.coeffs[0];
    const Datatype b = poly.coeffs[1];
    const Datatype c = poly.coeffs[2] - y;
    const Datatype disc = discriminant(poly);
    if (disc < 0.0) {
        throw std::runtime_error("The given polynomial has imaginary roots. Use complex_roots() instead.");
    }
    const Datatype root_term = std::sqrt(disc);
    const Datatype inv_2a = 1.0 / (2.0 * a);
    return {inv_2a * (-b - root_term), inv_2a * (-b + root_term)};
}

} // namespace nsl
