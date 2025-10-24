/*  __    __   ______   __
 * |  \  |  \ /      \ |  \
 * | &&\ | &&|  &&&&&&\| &&
 * | &&&\| &&| &&___\&&| &&
 * | &&&&\ && \&&    \ | &&
 * | &&\&& && _\&&&&&&\| &&
 * | && \&&&&|  \__| &&| &&_____
 * | &&  \&&& \&&    &&| &&     \
 *  \&&   \&&  \&&&&&&  \&&&&&&&&
 *
 *     Nino Standard Library
 *
 * Author: Nino Tarantino
 * Purpose: C++20 support for std::zip
 */
#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>

namespace nsl::detail {

// Iterator traits.
template <typename... Views>
concept AllRandomAccessRanges = (std::ranges::random_access_range<Views> && ...);
template <typename... Views>
concept AllForwardRanges = (std::ranges::forward_range<Views> && ...);
template <typename... Views>
concept AllBidirectionalRanges = (std::ranges::bidirectional_range<Views> && ...);

// Returns true if any elements in the left-hand side tuple are equal to the corresponding
// element in the right-hand side tuple.
template <typename Tuple, std::size_t... I>
bool any_tuple_element_equal(Tuple&& lhs, Tuple&& rhs, std::index_sequence<I...>) {
    return ((std::get<I>(std::forward<Tuple>(lhs)) == std::get<I>(std::forward<Tuple>(rhs))) || ...);
}

// A range adaptor that takes one or more views and produces a view whose ith element is a
// tuple containing the ith elements of all adapted views. Its size is the minimum size of
// all provided views.
template <std::ranges::input_range... Views>
requires (std::ranges::view<Views> && ...) && (sizeof...(Views) > 0)
class zip_view : public std::ranges::view_interface<zip_view<Views...>> {
public:
    class iterator;

    constexpr zip_view() = default;

    constexpr explicit zip_view(Views... args) : m_data(std::move(args)...) {}

    constexpr iterator begin() {
        auto get_begins = [] <typename... Elements>(Elements&&... el) {
            return std::make_tuple(std::begin(std::forward<Elements>(el))...);
        };
        return iterator {std::apply(get_begins, m_data)};
    }

    constexpr iterator end() {
        auto get_ends = [] <typename... Elements>(Elements&&... el) {
            return std::make_tuple(std::end(std::forward<Elements>(el))...);
        };
        return iterator {std::apply(get_ends, m_data)};
    }

private:
    std::tuple<Views...> m_data;
};

// A zip_view's iterator, which wraps a tuple of each adapted view's iterator types.
template<std::ranges::input_range... Views>
requires (std::ranges::view<Views> && ...) && (sizeof...(Views) > 0)
class zip_view<Views...>::iterator {
public:
    static constexpr auto get_iterator_category() {
        if constexpr (AllRandomAccessRanges<Views...>) {
            return std::random_access_iterator_tag {};
        } else if constexpr (AllBidirectionalRanges<Views...>) {
            return std::bidirectional_iterator_tag {};
        } else if constexpr (AllForwardRanges<Views...>) {
            return std::forward_iterator_tag {};
        } else {
            return std::input_iterator_tag {};
        }
    }

    using data_type = std::tuple<std::ranges::iterator_t<Views>...>;

    using difference_type = std::common_type_t<std::iter_difference_t<std::ranges::iterator_t<Views>>...>;
    using value_type = std::tuple<std::iter_value_t<std::ranges::iterator_t<Views>>...>;
    using pointer = std::tuple<typename std::iterator_traits<std::ranges::iterator_t<Views>>::pointer...>;
    using reference = std::tuple<std::iter_reference_t<std::ranges::iterator_t<Views>>...>;
    using iterator_category = decltype(get_iterator_category());

    constexpr explicit iterator(data_type args) : m_data(std::move(args)) {}

    constexpr iterator& operator++() {
        auto increment = [this] <typename... Elements>(Elements&&... el) {
            m_data = std::make_tuple(++std::forward<Elements>(el)...);
        };
        std::apply(increment, m_data);
        return *this;
    }

    constexpr iterator operator++(int)
    requires AllForwardRanges<Views...> {
        iterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr iterator& operator--()
    requires AllBidirectionalRanges<Views...> {
        auto decrement = [this] <typename... Elements>(Elements&&... el) {
            m_data = std::make_tuple(--std::forward<Elements>(el)...);
        };
        std::apply(decrement, m_data);
        return *this;
    }

    constexpr iterator operator--(int) {
        iterator ret = *this;
        --(*this);
        return ret;
    }

    constexpr value_type operator[](const difference_type index) const
    requires AllRandomAccessRanges<Views...> {
        auto offset = [this, index] <typename... Elements>(Elements&&... el) {
            return std::make_tuple(std::forward<Elements>(el)[index]...);
        };
        return std::apply(offset, m_data);
    }

    constexpr bool operator==(const iterator& other) const
    requires (std::equality_comparable<std::ranges::iterator_t<Views>> && ...) {
        return detail::any_tuple_element_equal(
            m_data, other.m_data, std::index_sequence_for<Views...>{});
    }

    constexpr reference operator*() {
        auto get_values = [] <typename... Elements>(Elements&&... el) {
            return std::forward_as_tuple(*std::forward<Elements>(el)...);
        };
        return std::apply(get_values, m_data);
    }

private:
    data_type m_data;
};

// Helper concept for better error messages when provided argument(s) to zip aren't ranges.
template <typename... Args>
concept ValidZipView = requires {
    zip_view<std::views::all_t<Args>...>(std::declval<Args>()...);
};

// A factory for creating a zip_view in the case where at least one input viewable range is
// provided, or an empty view if no arguments are provided.
class zip_factory {
public:
    template <typename... Args>
    requires (sizeof...(Args) == 0 || ValidZipView<Args...>)
    [[nodiscard]] constexpr auto operator()(Args&&... args) const {
        if constexpr (sizeof...(Args) == 0) {
            return std::views::empty<std::tuple<>>;
        } else {
            return zip_view<std::views::all_t<Args>...>(std::forward<Args>(args)...);
        }
    }
};

} // namespace nsl::detail
