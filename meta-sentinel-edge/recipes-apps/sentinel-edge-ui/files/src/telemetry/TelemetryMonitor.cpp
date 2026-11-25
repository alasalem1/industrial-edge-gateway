#include "TelemetryMonitor.h"

#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

TelemetryMonitor::TelemetryMonitor(QObject *parent)
    : QObject(parent)
{
    m_filePath = qEnvironmentVariable(
        "SENTINEL_EDGE_TELEMETRY_FILE",
        "/run/sentinel-edge/telemetry.json"
    );

    connect(&m_timer, &QTimer::timeout,
            this, &TelemetryMonitor::readTelemetry);

    m_timer.setInterval(1000);
    readTelemetry();
    m_timer.start();
}

bool TelemetryMonitor::connected() const
{
    return m_connected;
}

QString TelemetryMonitor::status() const
{
    return m_status;
}

QString TelemetryMonitor::source() const
{
    return m_source;
}

double TelemetryMonitor::temperature() const
{
    return m_temperature;
}

double TelemetryMonitor::pressure() const
{
    return m_pressure;
}

double TelemetryMonitor::vibration() const
{
    return m_vibration;
}

int TelemetryMonitor::motorRpm() const
{
    return m_motorRpm;
}

void TelemetryMonitor::setUnavailable(const QString &status)
{
    const bool changed =
        m_connected ||
        m_status != status ||
        m_source != "None";

    m_connected = false;
    m_status = status;
    m_source = "None";

    if (changed)
        emit telemetryUpdated();
}

void TelemetryMonitor::readTelemetry()
{
    QFile file(m_filePath);

    if (!file.exists()) {
        setUnavailable("NOT CONNECTED");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        setUnavailable("READ ERROR");
        return;
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError ||
        !document.isObject()) {
        setUnavailable("INVALID DATA");
        return;
    }

    const QJsonObject object = document.object();

    if (object.value("schema_version").toInt(-1) != 1) {
        setUnavailable("INVALID DATA");
        return;
    }

    const QJsonValue timestampValue = object.value("timestamp");
    const QJsonValue sourceValue = object.value("source");
    const QJsonValue temperatureValue = object.value("temperature_c");
    const QJsonValue pressureValue = object.value("pressure_bar");
    const QJsonValue vibrationValue = object.value("vibration_mm_s");
    const QJsonValue rpmValue = object.value("motor_rpm");

    if (!timestampValue.isDouble() ||
        !sourceValue.isString() ||
        !temperatureValue.isDouble() ||
        !pressureValue.isDouble() ||
        !vibrationValue.isDouble() ||
        !rpmValue.isDouble()) {
        setUnavailable("INVALID DATA");
        return;
    }

    const qint64 timestamp = static_cast<qint64>(
        timestampValue.toDouble()
    );

    const qint64 age =
        QDateTime::currentMSecsSinceEpoch() - timestamp;

    if (age < -5000 || age > 10000) {
        setUnavailable("STALE DATA");
        return;
    }

    const double temperature = temperatureValue.toDouble();
    const double pressure = pressureValue.toDouble();
    const double vibration = vibrationValue.toDouble();
    const double rpm = rpmValue.toDouble();

    if (temperature < -100.0 || temperature > 500.0 ||
        pressure < 0.0 || pressure > 1000.0 ||
        vibration < 0.0 || vibration > 1000.0 ||
        rpm < 0.0 || rpm > 100000.0 ||
        rpm != static_cast<int>(rpm) ||
        sourceValue.toString().trimmed().isEmpty()) {
        setUnavailable("INVALID DATA");
        return;
    }

    m_connected = true;
    m_status = "CONNECTED";
    m_source = sourceValue.toString();

    m_temperature = temperature;
    m_pressure = pressure;
    m_vibration = vibration;
    m_motorRpm = static_cast<int>(rpm);

    emit telemetryUpdated();
}
