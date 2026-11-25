#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "system/SystemMonitor.h"
#include "telemetry/TelemetryMonitor.h"
#include "faults/FaultManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setApplicationName("SentinelEdge Control Center");
    app.setOrganizationName("SentinelEdge");

    SystemMonitor systemMonitor;
    TelemetryMonitor telemetryMonitor;
    FaultManager faultManager;

    QObject::connect(
        &telemetryMonitor,
        &TelemetryMonitor::telemetryUpdated,
        &faultManager,
        [&telemetryMonitor, &faultManager]() {
            faultManager.processTelemetry(
                telemetryMonitor.connected(),
                telemetryMonitor.faultMask(),
                telemetryMonitor.diagnosticStatus(),
                telemetryMonitor.diagnosticSummary());
        });

    faultManager.processTelemetry(
        telemetryMonitor.connected(),
        telemetryMonitor.faultMask(),
        telemetryMonitor.diagnosticStatus(),
        telemetryMonitor.diagnosticSummary());

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/"));

    engine.rootContext()->setContextProperty(
        "systemMonitor",
        &systemMonitor
    );

    engine.rootContext()->setContextProperty(
        "telemetryMonitor",
        &telemetryMonitor
    );

    engine.rootContext()->setContextProperty(
        "faultManager",
        &faultManager
    );

    const QUrl mainUrl(QStringLiteral("qrc:/SentinelEdge/Main.qml"));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [mainUrl](QObject *object, const QUrl &url) {
            if (!object && url == mainUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(mainUrl);

    return app.exec();
}
