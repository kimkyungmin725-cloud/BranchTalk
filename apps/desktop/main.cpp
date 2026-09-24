#include "window_state_store.hpp"

#include <QColor>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QVariantMap>

#include <cstdlib>

namespace branchtalk::desktop
{
    namespace
    {

        bool theme_updates_immediately(QObject *root_object)
        {
            auto *panel = root_object->findChild<QObject *>(QStringLiteral("mainPanel"));
            auto *toggle = root_object->findChild<QObject *>(QStringLiteral("themeToggle"));
            if (panel == nullptr || toggle == nullptr || root_object->property("darkMode").toBool())
            {
                return false;
            }

            const auto initial_window_color = root_object->property("color").value<QColor>();
            const auto initial_panel_color = panel->property("color").value<QColor>();
            const auto initial_button_text = toggle->property("text").toString();

            const bool invoked =
                QMetaObject::invokeMethod(root_object, "toggleTheme", Qt::DirectConnection);

            const auto updated_window_color = root_object->property("color").value<QColor>();
            const auto updated_panel_color = panel->property("color").value<QColor>();
            const auto updated_button_text = toggle->property("text").toString();

            return invoked && root_object->property("darkMode").toBool() &&
                   initial_window_color.isValid() && updated_window_color.isValid() &&
                   initial_panel_color.isValid() && updated_panel_color.isValid() &&
                   initial_window_color != updated_window_color &&
                   initial_panel_color != updated_panel_color &&
                   initial_button_text != updated_button_text;
        }
    } // namespace

    int run(int argc, char *argv[])
    {
        QGuiApplication application{argc, argv};
        QCoreApplication::setOrganizationName(QStringLiteral("BranchTalk"));
        QCoreApplication::setApplicationName(QStringLiteral("BranchTalk"));

        const bool smoke_test = application.arguments().contains(QStringLiteral("--smoke-test"));
        const bool theme_test = application.arguments().contains(QStringLiteral("--theme-test"));
        const bool test_mode = smoke_test || theme_test;
        QSettings settings;
        WindowStateStore window_state_store{settings};

        QList<QRect> available_screens;
        for (const auto *screen : QGuiApplication::screens())
        {
            available_screens.append(screen->availableGeometry());
        }

        QQmlApplicationEngine engine;

        if (!test_mode)
        {
            if (const auto restored_state = window_state_store.restore(available_screens))
            {
                QVariantMap initial_properties;
                initial_properties.insert(QStringLiteral("x"), restored_state->x);
                initial_properties.insert(QStringLiteral("y"), restored_state->y);
                initial_properties.insert(QStringLiteral("width"), restored_state->width);
                initial_properties.insert(QStringLiteral("height"), restored_state->height);
                engine.setInitialProperties(initial_properties);
            }
        }

        QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &application, []
                         { QCoreApplication::exit(EXIT_FAILURE); }, Qt::QueuedConnection);

        engine.loadFromModule("BranchTalk", "Main");
        if (engine.rootObjects().isEmpty())
        {
            return EXIT_FAILURE;
        }

        if (!test_mode)
        {
            const QPointer<QObject> root_object{engine.rootObjects().constFirst()};
            QObject::connect(&application,
                             &QCoreApplication::aboutToQuit,
                             &application,
                             [&window_state_store, root_object]
                             {
                                 if (root_object.isNull())
                                 {
                                     return;
                                 }

                                 window_state_store.save(WindowState{
                                     root_object->property("x").toInt(),
                                     root_object->property("y").toInt(),
                                     root_object->property("width").toInt(),
                                     root_object->property("height").toInt(),
                                 });
                             });
        }

        if (theme_test)
        {
            const bool updated = theme_updates_immediately(engine.rootObjects().constFirst());
            QTimer::singleShot(0, &application, [updated]
                               { QCoreApplication::exit(updated ? EXIT_SUCCESS : EXIT_FAILURE); });
        }
        else if (smoke_test)
        {
            QTimer::singleShot(0, &application, &QCoreApplication::quit);
        }

        return application.exec();
    }
} // namespace branchtalk::exec();

int main(int argc, char *argv[])
{
    return branchtalk::desktop::run(argc, argv);
}