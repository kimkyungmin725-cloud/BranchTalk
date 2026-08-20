#pragma

#include <branchtalk/core/error.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

namespace branchtalk::core
{

    template <typename T>
    class [[nodiscard]] Result
    {
        static_assert(!std::is_reference_v<T>, "Result<T> cannot store a reference");
        static_assert(!std::is_void_v<T>, "Result<void> is not supported");

    public:
        [[nodiscard]] static Result success(T value)
        {
            return Result(std::in_place_index<0>, std::move(value));
        }

        [[nodiscard]] static Result failure(Error error)
        {
            return Result(std::in_place_index<1>, std::move(error));
        }

        [[nodiscard]] bool has_value() const noexcept
        {
            return data_.index() == 0;
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return has_value();
        }

        [[nodiscard]] T *value_if() noexcept
        {
            return std::get_if<0>(&data_);
        }

        [[nodiscard]] const T *value_if() const
        {
            return std::get_if<0>(&data_);
        }

        [[nodiscard]] Error *error_if() noexcept
        {
            return std::get_if<1>(&data_);
        }

        [[nodiscard]] const Error *error_if() const
        {
            return std::get_if<1>(&data_);
        }

    private:
        template <std::size_t Index, typename Value>
        explicit Result(std::in_place_index_t<Index> index, Value &&value)
            : data_(index, std::forward<Value>(value))
        {
        }

        std::variant<T, Error> data_;
    };
} // namespace branchtalk::core