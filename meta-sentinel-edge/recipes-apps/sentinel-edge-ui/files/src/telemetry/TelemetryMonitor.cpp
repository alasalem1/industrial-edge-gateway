#include "TelemetryMonitor.h"

#include "diagnostics/diagnostic_engine.h"
#include "ml/anomaly_detector.h"

#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QStringList>

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

QString TelemetryMonitor::diagnosticStatus() const
{
    return m_diagnosticStatus;
}

QString TelemetryMonitor::diagnosticSummary() const
{
    return m_diagnosticSummary;
}

int TelemetryMonitor::faultMask() const
{
    return m_faultMask;
}

QString TelemetryMonitor::aiStatus() const
{
    return m_aiStatus;
}

QString TelemetryMonitor::aiSummary() const
{
    return m_aiSummary;
}

double TelemetryMonitor::aiAnomalyScore() const
{
    return m_aiAnomalyScore;
}

void TelemetryMonitor::setUnavailable(const QString &status)
{
    const bool changed =
        m_connected ||
        m_status != status ||
        m_source != "None" ||
        m_diagnosticStatus != "UNKNOWN" ||
        m_diagnosticSummary != "Waiting for valid telemetry" ||
        m_aiStatus != "UNKNOWN" ||
        m_aiSummary != "Waiting for valid telemetry";

    m_connected = false;
    m_status = status;
    m_source = "None";

    m_diagnosticStatus = "UNKNOWN";
    m_diagnosticSummary = "Waiting for valid telemetry";
    m_faultMask = 0;
    m_aiStatus = "UNKNOWN";
    m_aiSummary = "Waiting for valid telemetry";
    m_aiAnomalyScore = 0.0;

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

    const double aiFeatures[SENTINEL_AI_FEATURE_COUNT] = {
        m_temperature,
        m_pressure,
        m_vibration,
        static_cast<double>(m_motorRpm)
    };

    double aiScore = 0.0;
    const int aiResult = sentinel_ai_predict(aiFeatures, &aiScore);

    if (aiResult >= 0) {
        m_aiAnomalyScore = aiScore;
        m_aiStatus = aiResult == 1 ? "ANOMALY" : "NORMAL";
        m_aiSummary = QString("%1 (score: %2)")
            .arg(aiResult == 1
                ? "Unusual sensor pattern detected"
                : "No unusual pattern detected")
            .arg(aiScore, 0, 'f', 4);
    } else {
        m_aiStatus = "ERROR";
        m_aiSummary = "ML inference failed";
        m_aiAnomalyScore = 0.0;
    }

    DiagnosticInput input = {
        m_temperature,
        m_pressure,
        m_vibration,
        m_motorRpm
    };

    DiagnosticResult result = {};

    if (diagnostic_evaluate(&input, &result) != 0) {
        m_faultMask = 0;
        m_diagnosticStatus = "UNKNOWN";
        m_diagnosticSummary = "Diagnostic evaluation failed";
        emit telemetryUpdated();
        return;
    }

    m_faultMask = static_cast<int>(result.faults);

    switch (result.severity) {
    case DIAGNOSTIC_NORMAL:
        m_diagnosticStatus = "NORMAL";
        break;

    case DIAGNOSTIC_WARNING:
        m_diagnosticStatus = "WARNING";
        break;

    case DIAGNOSTIC_CRITICAL:
        m_diagnosticStatus = "CRITICAL";
        break;

    default:
        m_diagnosticStatus = "UNKNOWN";
        break;
    }

    QStringList faults;

    if (result.faults & FAULT_TEMPERATURE)
        faults << "High temperature";

    if (result.faults & FAULT_PRESSURE)
        faults << "High pressure";

    if (result.faults & FAULT_VIBRATION)
        faults << "High vibration";

    if (result.faults & FAULT_MOTOR_SPEED)
        faults << "High motor speed";

    m_diagnosticSummary = faults.isEmpty()
        ? "No threshold violations"
        : faults.join(" / ");

    emit telemetryUpdated();
}
