#include "app_view_model.hpp"
#include "window_state_store.hpp"
#include "workspace_list_model.hpp"

#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QVariantMap>

#include <cmath>
#include <cstdlib>

namespace branchtalk::desktop
{
    namespace
    {

        bool theme_updates_immediately(QObject *root_object, AppViewModel &view_model)
        {
            view_model.showMain();
            QCoreApplication::processEvents();

            auto *panel = root_object->findChild<QObject *>(QStringLiteral("conversationPanel"));
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

        bool layout_updates_stably(QObject *root_object, AppViewModel &view_model)
        {
            view_model.showMain();
            QCoreApplication::processEvents();

            auto *workspace_panel =
                root_object->findChild<QObject *>(QStringLiteral("workspacePanel"));
            auto *channel_panel = root_object->findChild<QObject *>(QStringLiteral("channelPanel"));
            auto *conversation_panel =
                root_object->findChild<QObject *>(QStringLiteral("conversationPanel"));
            auto *workspace_list =
                root_object->findChild<QObject *>(QStringLiteral("workspaceList"));
            auto *channel_action = root_object->findChild<QObject *>(QStringLiteral("channelAction"));

            if (workspace_panel == nullptr || channel_panel == nullptr ||
                conversation_panel == nullptr || workspace_list == nullptr ||
                channel_action == nullptr)
            {
                return false;
            }

            const double workspace_minimum =
                root_object->property("workspaceMinimumWidth").toDouble();
            const double channel_minimum = root_object->property("channelMinimumWidth").toDouble();
            const double conversation_minimum =
                root_object->property("conversationMinimumWidth").toDouble();

            const auto resize_window = [root_object](int width)
            {
                root_object->setProperty("width", width);
                QCoreApplication::processEvents();
            };
            const auto width_of = [](const QObject *object)
            {
                return object->property("width").toDouble();
            };

            resize_window(1000);
            const double initial_workspace_width = width_of(workspace_panel);
            const double initial_channel_width = width_of(channel_panel);
            const bool wide_layout_valid =
                !root_object->property("compactLayout").toBool() &&
                conversation_panel->property("visible").toBool() &&
                initial_workspace_width >= workspace_minimum &&
                initial_channel_width >= channel_minimum &&
                width_of(conversation_panel) >= conversation_minimum;

            resize_window(700);
            const bool compact_layout_valid =
                root_object->property("compactLayout").toBool() &&
                !conversation_panel->property("visible").toBool() &&
                workspace_panel->property("visible").toBool() &&
                channel_panel->property("visible").toBool() &&
                width_of(workspace_panel) >= workspace_minimum &&
                width_of(channel_panel) >= channel_minimum &&
                workspace_list->property("visible").toBool() &&
                workspace_list->property("enabled").toBool() &&
                workspace_list->property("count").toInt() > 0 &&
                channel_action->property("visible").toBool() &&
                channel_action->property("enabled").toBool();

            resize_window(1000);
            constexpr double resize_tolerance = 1.0;
            const bool restored_layout_valid =
                !root_object->property("compactLayout").toBool() &&
                conversation_panel->property("visible").toBool() &&
                std::abs(width_of(workspace_panel) - initial_workspace_width) <= resize_tolerance &&
                std::abs(width_of(channel_panel) - initial_channel_width) <= resize_tolerance &&
                width_of(conversation_panel) >= conversation_minimum;

            const bool layout_valid =
                wide_layout_valid && compact_layout_valid && restored_layout_valid;
            if (!layout_valid)
            {
                qWarning() << "layout verification failed"
                           << "wide" << wide_layout_valid
                           << "compact" << compact_layout_valid
                           << "restored" << restored_layout_valid
                           << "workspace" << width_of(workspace_panel)
                           << "channel" << width_of(channel_panel)
                           << "conversation" << width_of(conversation_panel);
            }

            return layout_valid;
        }

        bool navigation_follows_view_model(QObject *root_object, AppViewModel &view_model)
        {
            auto *login_view = root_object->findChild<QObject *>(QStringLiteral("loginView"));
            auto *main_view = root_object->findChild<QObject *>(QStringLiteral("mainView"));
            auto *settings_view = root_object->findChild<QObject *>(QStringLiteral("settingsView"));
            auto *enter_main_action =
                root_object->findChild<QObject *>(QStringLiteral("enterMainAction"));
            auto *open_settings_action =
                root_object->findChild<QObject *>(QStringLiteral("openSettingsAction"));
            auto *settings_back_action =
                root_object->findChild<QObject *>(QStringLiteral("settingsBackAction"));
            auto *return_to_login_action =
                root_object->findChild<QObject *>(QStringLiteral("returnToLoginAction"));

            if (login_view == nullptr || main_view == nullptr || settings_view == nullptr ||
                enter_main_action == nullptr || open_settings_action == nullptr ||
                settings_back_action == nullptr || return_to_login_action == nullptr ||
                root_object->property("appViewModel").value<QObject *>() != &view_model)
            {
                return false;
            }

            const auto visible = [](const QObject *view)
            {
                return view->property("visible").toBool();
            };
            const auto only_view_is_visible =
                [&visible, login_view, main_view, settings_view](const QObject *expected)
            {
                return visible(login_view) == (expected == login_view) &&
                       visible(main_view) == (expected == main_view) &&
                       visible(settings_view) == (expected == settings_view);
            };
            const auto click = [](QObject *action)
            {
                const bool invoked =
                    QMetaObject::invokeMethod(action, "clicked", Qt::DirectConnection);
                QCoreApplication::processEvents();
                return invoked;
            };

            if (view_model.currentScreen() != AppViewModel::LoginScreen ||
                !only_view_is_visible(login_view))
            {
                return false;
            }

            if (!click(enter_main_action) || view_model.currentScreen() != AppViewModel::MainScreen ||
                !only_view_is_visible(main_view))
            {
                return false;
            }

            if (!click(open_settings_action) ||
                view_model.currentScreen() != AppViewModel::SettingsScreen ||
                !only_view_is_visible(settings_view))
            {
                return false;
            }

            if (!click(settings_back_action) ||
                view_model.currentScreen() != AppViewModel::MainScreen ||
                !only_view_is_visible(main_view) || !click(open_settings_action) ||
                !only_view_is_visible(settings_view))
            {
                return false;
            }

            return click(return_to_login_action) &&
                   view_model.currentScreen() == AppViewModel::LoginScreen &&
                   only_view_is_visible(login_view);
        }

        bool workspace_sidebar_updates(QObject *root_object,
                                       WorkspaceListModel &workspace_model)
        {
            const auto failed = [](const char *stage)
            {
                qWarning() << "workspace sidebar verification failed" << stage;
                return false;
            };

            QCoreApplication::processEvents();

            auto *workspace_list =
                root_object->findChild<QObject *>(QStringLiteral("workspaceList"));
            auto *loading_state =
                root_object->findChild<QObject *>(QStringLiteral("workspaceLoadingState"));
            auto *empty_state =
                root_object->findChild<QObject *>(QStringLiteral("workspaceEmptyState"));
            if (workspace_list == nullptr || loading_state == nullptr || empty_state == nullptr ||
                root_object->property("workspaceModel").value<QObject *>() != &workspace_model ||
                workspace_model.selectedWorkspaceId() != QStringLiteral("workspace-bt") ||
                workspace_list->property("count").toInt() != 3 ||
                !workspace_list->property("visible").toBool() ||
                loading_state->property("visible").toBool() ||
                empty_state->property("visible").toBool())
            {
                qWarning() << "workspace sidebar initial state"
                           << "list" << workspace_list
                           << "loading" << loading_state
                           << "empty" << empty_state
                           << "selected" << workspace_model.selectedWorkspaceId()
                           << "count"
                           << (workspace_list == nullptr ? -1
                                                         : workspace_list->property("count").toInt());
                return failed("initial state");
            }

            if (!QMetaObject::invokeMethod(workspace_list, "forceLayout", Qt::DirectConnection))
            {
                return failed("force initial list layout");
            }
            QCoreApplication::processEvents();

            const auto item_at = [workspace_list](int row) -> QObject *
            {
                QQuickItem *item = nullptr;
                const bool invoked =
                    QMetaObject::invokeMethod(workspace_list,
                                              "itemAtIndex",
                                              Qt::DirectConnection,
                                              Q_RETURN_ARG(QQuickItem *, item),
                                              Q_ARG(int, row));
                return invoked ? item : nullptr;
            };
            const auto click = [](QObject *action)
            {
                if (action == nullptr ||
                    !QMetaObject::invokeMethod(action, "clicked", Qt::DirectConnection))
                {
                    return false;
                }
                QCoreApplication::processEvents();
                return true;
            };

            auto *bt_action = item_at(0);
            auto *dev_action = item_at(1);
            if (bt_action == nullptr || dev_action == nullptr ||
                !bt_action->property("active").toBool() ||
                dev_action->property("active").toBool() ||
                dev_action->property("text").toString() != QStringLiteral("DEV · 4"))
            {
                qWarning() << "workspace delegates"
                           << "bt" << bt_action
                           << "dev" << dev_action
                           << "bt active"
                           << (bt_action == nullptr ? false : bt_action->property("active").toBool())
                           << "dev active"
                           << (dev_action == nullptr ? false
                                                     : dev_action->property("active").toBool())
                           << "dev text"
                           << (dev_action == nullptr ? QString{}
                                                     : dev_action->property("text").toString());
                return failed("initial delegates");
            }
            if (!click(dev_action) ||
                workspace_model.selectedWorkspaceId() != QStringLiteral("workspace-dev") ||
                !dev_action->property("active").toBool())
            {
                qWarning() << "workspace selection"
                           << "selected" << workspace_model.selectedWorkspaceId()
                           << "dev active" << dev_action->property("active").toBool();
                return failed("selection");
            }

            if (!workspace_model.addWorkspace(QStringLiteral("workspace-qa"),
                                              QStringLiteral("QA"),
                                              7))
            {
                return failed("add workspace");
            }
            QCoreApplication::processEvents();
            QMetaObject::invokeMethod(workspace_list, "forceLayout", Qt::DirectConnection);
            QCoreApplication::processEvents();
            auto *qa_action = item_at(3);
            if (workspace_list->property("count").toInt() != 4 || qa_action == nullptr ||
                qa_action->property("text").toString() != QStringLiteral("QA · 7") ||
                !workspace_model.setUnreadCount(QStringLiteral("workspace-qa"), 2))
            {
                return failed("inserted delegate");
            }
            QCoreApplication::processEvents();
            if (qa_action->property("text").toString() != QStringLiteral("QA · 2") ||
                !workspace_model.removeWorkspace(QStringLiteral("workspace-bt")))
            {
                return failed("unread update or removal");
            }
            QCoreApplication::processEvents();
            QMetaObject::invokeMethod(workspace_list, "forceLayout", Qt::DirectConnection);
            QCoreApplication::processEvents();
            dev_action = item_at(0);
            if (workspace_list->property("count").toInt() != 3 || dev_action == nullptr ||
                dev_action->property("stableId").toString() != QStringLiteral("workspace-dev") ||
                workspace_model.selectedWorkspaceId() != QStringLiteral("workspace-dev") ||
                !dev_action->property("active").toBool())
            {
                qWarning() << "workspace shifted selection"
                           << "count" << workspace_list->property("count")
                           << "item" << dev_action
                           << "item id"
                           << (dev_action == nullptr ? QString{}
                                                     : dev_action->property("stableId").toString())
                           << "selected" << workspace_model.selectedWorkspaceId()
                           << "active"
                           << (dev_action == nullptr ? false
                                                     : dev_action->property("active").toBool());
                return failed("stable selection after row shift");
            }

            workspace_model.setLoading(true);
            QCoreApplication::processEvents();
            if (!loading_state->property("visible").toBool() ||
                workspace_list->property("visible").toBool() ||
                empty_state->property("visible").toBool())
            {
                return failed("loading state");
            }

            workspace_model.setLoading(false);
            QCoreApplication::processEvents();
            if (!workspace_list->property("visible").toBool() ||
                loading_state->property("visible").toBool())
            {
                return failed("loaded state");
            }

            if (!workspace_model.removeWorkspace(QStringLiteral("workspace-ops")) ||
                !workspace_model.removeWorkspace(QStringLiteral("workspace-qa")) ||
                !workspace_model.removeWorkspace(QStringLiteral("workspace-dev")))
            {
                return failed("remove all workspaces");
            }
            QCoreApplication::processEvents();
            const bool empty_state_valid =
                workspace_model.empty() && workspace_model.selectedWorkspaceId().isEmpty() &&
                workspace_list->property("count").toInt() == 0 &&
                !workspace_list->property("visible").toBool() &&
                !loading_state->property("visible").toBool() &&
                empty_state->property("visible").toBool();
            return empty_state_valid ? true : failed("empty state");
        }

    } // namespace

    int run(int argc, char *argv[])
    {
        QGuiApplication application{argc, argv};
        QCoreApplication::setOrganizationName(QStringLiteral("BranchTalk"));
        QCoreApplication::setApplicationName(QStringLiteral("BranchTalk"));

        const bool smoke_test = application.arguments().contains(QStringLiteral("--smoke-test"));
        const bool theme_test = application.arguments().contains(QStringLiteral("--theme-test"));
        const bool layout_test = application.arguments().contains(QStringLiteral("--layout-test"));
        const bool navigation_test =
            application.arguments().contains(QStringLiteral("--navigation-test"));
        const bool workspace_model_test =
            application.arguments().contains(QStringLiteral("--workspace-model-test"));
        const bool test_mode =
            smoke_test || theme_test || layout_test || navigation_test || workspace_model_test;
        QSettings settings;
        WindowStateStore window_state_store{settings};

        QList<QRect> available_screens;
        for (const auto *screen : QGuiApplication::screens())
        {
            available_screens.append(screen->availableGeometry());
        }

        AppViewModel view_model;
        WorkspaceListModel workspace_model;
        if (!workspace_model.addWorkspace(
                QStringLiteral("workspace-bt"), QStringLiteral("BT"), 0) ||
            !workspace_model.addWorkspace(
                QStringLiteral("workspace-dev"), QStringLiteral("DEV"), 4) ||
            !workspace_model.addWorkspace(
                QStringLiteral("workspace-ops"), QStringLiteral("OPS"), 1) ||
            !workspace_model.selectWorkspace(QStringLiteral("workspace-bt")))
        {
            return EXIT_FAILURE;
        }

        QQmlApplicationEngine engine;
        QVariantMap initial_properties;
        initial_properties.insert(QStringLiteral("appViewModel"), QVariant::fromValue(&view_model));
        initial_properties.insert(QStringLiteral("workspaceModel"),
                                  QVariant::fromValue(static_cast<QObject *>(&workspace_model)));

        if (!test_mode)
        {
            if (const auto restored_state = window_state_store.restore(available_screens))
            {
                initial_properties.insert(QStringLiteral("x"), restored_state->x);
                initial_properties.insert(QStringLiteral("y"), restored_state->y);
                initial_properties.insert(QStringLiteral("width"), restored_state->width);
                initial_properties.insert(QStringLiteral("height"), restored_state->height);
            }
        }
        engine.setInitialProperties(initial_properties);

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

        if (workspace_model_test)
        {
            QObject *root_object = engine.rootObjects().constFirst();
            view_model.showMain();
            QTimer::singleShot(0, &application, [&application, root_object, &workspace_model]
                               {
            const bool updated = workspace_sidebar_updates(root_object, workspace_model);
            QCoreApplication::exit(updated ? EXIT_SUCCESS : EXIT_FAILURE); });
        }
        else if (navigation_test)
        {
            const bool updated =
                navigation_follows_view_model(engine.rootObjects().constFirst(), view_model);
            QTimer::singleShot(0, &application, [updated]
                               { QCoreApplication::exit(updated ? EXIT_SUCCESS : EXIT_FAILURE); });
        }
        else if (layout_test)
        {
            const bool updated =
                layout_updates_stably(engine.rootObjects().constFirst(), view_model);
            QTimer::singleShot(0, &application, [updated]
                               { QCoreApplication::exit(updated ? EXIT_SUCCESS : EXIT_FAILURE); });
        }
        else if (theme_test)
        {
            const bool updated =
                theme_updates_immediately(engine.rootObjects().constFirst(), view_model);
            QTimer::singleShot(0, &application, [updated]
                               { QCoreApplication::exit(updated ? EXIT_SUCCESS : EXIT_FAILURE); });
        }
        else if (smoke_test)
        {
            QTimer::singleShot(0, &application, &QCoreApplication::quit);
        }

        return application.exec();
    }

} // namespace branchtalk::desktop

int main(int argc, char *argv[])
{
    return branchtalk::desktop::run(argc, argv);
}
