#include "window_state_store.hpp"

#include <QSettings>
#include <QString>

#include <algorithm>
#include <cstdint>
#include <optional>

namespace branchtalk::desktop
{
    namespace
    {

        std::optional<int> read_integer(const QSettings &settings, const QString &key)
        {
            const auto value = settings.value(key);
            bool converted = false;
            const int integer = value.toInt(&converted);
            return converted ? std::optional{integer} : std::nullopt;
        }

        bool is_visible_on_screen(const WindowState &state, const QRect &screen)
        {
            if (!screen.isValid() || state.width > screen.width() || state.height > screen.height())
            {
                return false;
            }

            const auto window_left = static_cast<std::int64_t>(state.x);
            const auto window_top = static_cast<std::int64_t>(state.y);
            const auto window_right = window_left + state.width;
            const auto window_bottom = window_top + state.height;
            const auto screen_left = static_cast<std::int64_t>(screen.x());
            const auto screen_top = static_cast<std::int64_t>(screen.y());
            const auto screen_right = screen_left + screen.width();
            const auto screen_bottom = screen_top + screen.height();

            return window_left < screen_right && window_right > screen_left &&
                   window_top < screen_bottom && window_bottom > screen_top;
        }

    } // namespace

    WindowStateStore::WindowStateStore(QSettings &settings) noexcept : settings_{settings} {}

    std::optional<WindowState> WindowStateStore::restore(
        const QList<QRect> &available_screens) const
    {
        const auto x = read_integer(settings_, QStringLiteral("window/x"));
        const auto y = read_integer(settings_, QStringLiteral("window/y"));
        const auto width = read_integer(settings_, QStringLiteral("window/width"));
        const auto height = read_integer(settings_, QStringLiteral("window/height"));

        if (!x || !y || !width || !height || *width < minimum_width ||
            *height < minimum_height)
        {
            return std::nullopt;
        }

        const WindowState state{*x, *y, *width, *height};
        const bool visible = std::any_of(
            available_screens.cbegin(),
            available_screens.cend(),
            [&state](const QRect &screen)
            { return is_visible_on_screen(state, screen); });

        return visible ? std::optional{state} : std::nullopt;
    }

    void WindowStateStore::save(const WindowState &state)
    {
        settings_.setValue(QStringLiteral("window/x"), state.x);
        settings_.setValue(QStringLiteral("window/y"), state.y);
        settings_.setValue(QStringLiteral("window/width"), state.width);
        settings_.setValue(QStringLiteral("window/height"), state.height);
        settings_.sync();
    }

} // namespace branchtalk::desktop