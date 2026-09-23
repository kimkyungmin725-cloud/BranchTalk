#pragma once

#include <QList>
#include <QRect>

#include <optional>

class QSettings;

namespace branchtalk::desktop
{

    struct WindowState
    {
        int x;
        int y;
        int width;
        int height;

        bool operator==(const WindowState &) const = default;
    };

    class WindowStateStore
    {
    public:
        static constexpr int minimum_width = 640;
        static constexpr int minimum_height = 480;

        explicit WindowStateStore(QSettings &settings) noexcept;

        [[nodiscard]] std::optional<WindowState> restore(
            const QList<QRect> &available_screens) const;
        void save(const WindowState &state);

    private:
        QSettings &settings_;
    };

} // namespace branchtalk::desktop