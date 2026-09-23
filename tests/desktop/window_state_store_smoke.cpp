#include "window_state_store.hpp"

#include <QList>
#include <QRect>
#include <QSettings>
#include <QTemporaryDir>

namespace
{

    using branchtalk::desktop::WindowState;
    using branchtalk::desktop::WindowStateStore;

    const QList<QRect> available_screens{QRect{0, 0, 1920, 1080}};

    bool saved_state_is_restored()
    {
        QTemporaryDir directory;
        if (!directory.isValid())
        {
            return false;
        }

        const auto settings_path = directory.filePath(QStringLiteral("window-state.ini"));
        const WindowState expected{120, 80, 1024, 720};

        {
            QSettings settings{settings_path, QSettings::IniFormat};
            WindowStateStore store{settings};
            store.save(expected);
        }

        QSettings reloaded_settings{settings_path, QSettings::IniFormat};
        WindowStateStore reloaded_store{reloaded_settings};
        return reloaded_store.restore(available_screens) == expected;
    }

    bool missing_or_invalid_state_uses_defaults()
    {
        QTemporaryDir directory;
        if (!directory.isValid())
        {
            return false;
        }

        QSettings settings{directory.filePath(QStringLiteral("invalid-state.ini")),
                           QSettings::IniFormat};
        WindowStateStore store{settings};

        if (store.restore(available_screens).has_value())
        {
            return false;
        }

        settings.setValue(QStringLiteral("window/x"), 100);
        settings.setValue(QStringLiteral("window/y"), 100);
        settings.setValue(QStringLiteral("window/width"), QStringLiteral("not-a-number"));
        settings.setValue(QStringLiteral("window/height"), 600);
        if (store.restore(available_screens).has_value())
        {
            return false;
        }

        settings.setValue(QStringLiteral("window/width"), WindowStateStore::minimum_width - 1);
        if (store.restore(available_screens).has_value())
        {
            return false;
        }

        settings.setValue(QStringLiteral("window/x"), 4000);
        settings.setValue(QStringLiteral("window/y"), 4000);
        settings.setValue(QStringLiteral("window/width"), 800);
        settings.setValue(QStringLiteral("window/height"), 600);
        return !store.restore(available_screens).has_value();
    }

} // namespace

int main()
{
    return saved_state_is_restored() && missing_or_invalid_state_uses_defaults() ? 0 : 1;
}