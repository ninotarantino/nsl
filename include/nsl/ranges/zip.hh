#pragma once

#include <cstddef>
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>

namespace nsl {
namespace detail {

template <typename Tuple, std::size_t... I>
bool any_tuple_element_equal(Tuple&& lhs, Tuple&& rhs, std::index_sequence<I...>) {
    return ((std::get<I>(std::forward<Tuple>(lhs)) == std::get<I>(std::forward<Tuple>(rhs))) || ...);
}

}

template <typename... Types>
class zip {
    public:
        struct iterator {
            using difference_type = std::tuple<std::iter_difference_t<std::ranges::iterator_t<Types>>...>;
            using value_type = std::tuple<std::iter_value_t<std::ranges::iterator_t<Types>>...>;
            using pointer = std::tuple<typename std::iterator_traits<std::ranges::iterator_t<Types>>::pointer...>;
            using reference = std::tuple<std::iter_reference_t<std::ranges::iterator_t<Types>>...>;
            using iterator_category = std::input_iterator_tag;

            iterator& operator++() {
                auto increment = [this] <typename... Element>(Element&&... el) {
                    m_data = std::make_tuple(++std::forward<Element>(el)...);
                };
                std::apply(increment, m_data);
                return *this;
            }

            iterator operator++(int) {
                iterator ret = *this;
                ++(*this);
                return ret;
            }

            bool operator==(const iterator& other) const {
                return detail::any_tuple_element_equal(
                    m_data, other.m_data, std::index_sequence_for<Types...>{});
            }

            reference operator*() {
                auto get_values = [] <typename... Element>(Element&&... el) {
                    return std::forward_as_tuple(*std::forward<Element>(el)...);
                };
                return std::apply(get_values, m_data);
            }

            std::tuple<std::ranges::iterator_t<Types>...> m_data;
        };

        zip(Types&... args) : m_data(std::forward_as_tuple(std::forward<Types>(args)...)) {}

        iterator begin() {
            auto get_begins = [] <typename... Element>(Element&&... el) {
                return std::make_tuple(std::begin(std::forward<Element>(el))...);
            };
            return iterator {std::apply(get_begins, m_data)};
        }

        iterator end() {
            auto get_ends = [] <typename... Element>(Element&&... el) {
                return std::make_tuple(std::end(std::forward<Element>(el))...);
            };
            return iterator {std::apply(get_ends, m_data)};
        }

    private:
        std::tuple<Types&...> m_data;
};

}
