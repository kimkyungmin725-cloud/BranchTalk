#include "window_state_store.hpp"

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

    int run(int argc, char *argv[])
    {
        QGuiApplication application{argc, argv};
        QCoreApplication::setOrganizationName(QStringLiteral("BranchTalk"));
        QCoreApplication::setApplicationName(QStringLiteral("BranchTalk"));

        const bool smoke_test = application.arguments().contains(QStringLiteral("--smoke-test"));
        QSettings settings;
        WindowStateStore window_state_store{settings};

        QList<QRect> available_screens;
        for (const auto *screen : QGuiApplication::screens())
        {
            available_screens.append(screen->availableGeometry());
        }

        QQmlApplicationEngine engine;

        if (!smoke_test)
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

        if (!smoke_test)
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

        if (smoke_test)
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