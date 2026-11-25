#ifndef TELEMETRYMONITOR_H
#define TELEMETRYMONITOR_H

#include <QObject>
#include <QString>
#include <QTimer>

class TelemetryMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY telemetryUpdated)
    Q_PROPERTY(QString status READ status NOTIFY telemetryUpdated)
    Q_PROPERTY(QString source READ source NOTIFY telemetryUpdated)

    Q_PROPERTY(double temperature READ temperature NOTIFY telemetryUpdated)
    Q_PROPERTY(double pressure READ pressure NOTIFY telemetryUpdated)
    Q_PROPERTY(double vibration READ vibration NOTIFY telemetryUpdated)
    Q_PROPERTY(int motorRpm READ motorRpm NOTIFY telemetryUpdated)

    Q_PROPERTY(QString diagnosticStatus READ diagnosticStatus NOTIFY telemetryUpdated)
    Q_PROPERTY(QString diagnosticSummary READ diagnosticSummary NOTIFY telemetryUpdated)
    Q_PROPERTY(int faultMask READ faultMask NOTIFY telemetryUpdated)
    Q_PROPERTY(QString aiStatus READ aiStatus NOTIFY telemetryUpdated)
    Q_PROPERTY(QString aiSummary READ aiSummary NOTIFY telemetryUpdated)
    Q_PROPERTY(double aiAnomalyScore READ aiAnomalyScore NOTIFY telemetryUpdated)

public:
    explicit TelemetryMonitor(QObject *parent = nullptr);

    bool connected() const;
    QString status() const;
    QString source() const;

    double temperature() const;
    double pressure() const;
    double vibration() const;
    int motorRpm() const;

    QString diagnosticStatus() const;
    QString diagnosticSummary() const;
    int faultMask() const;
    QString aiStatus() const;
    QString aiSummary() const;
    double aiAnomalyScore() const;

signals:
    void telemetryUpdated();

private slots:
    void readTelemetry();

private:
    void setUnavailable(const QString &status);

    QTimer m_timer;

    QString m_filePath;
    QString m_status = "NOT CONNECTED";
    QString m_source = "None";

    QString m_diagnosticStatus = "UNKNOWN";
    QString m_diagnosticSummary = "Waiting for valid telemetry";

    bool m_connected = false;

    double m_temperature = 0.0;
    double m_pressure = 0.0;
    double m_vibration = 0.0;
    int m_motorRpm = 0;
    int m_faultMask = 0;
    QString m_aiStatus = "UNKNOWN";
    QString m_aiSummary = "Waiting for valid telemetry";
    double m_aiAnomalyScore = 0.0;
};

#endif
