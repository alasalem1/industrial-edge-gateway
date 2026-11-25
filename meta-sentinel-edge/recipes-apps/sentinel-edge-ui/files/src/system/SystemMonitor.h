
#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QtGlobal>

class SystemMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY metricsUpdated)
    Q_PROPERTY(double memoryUsage READ memoryUsage NOTIFY metricsUpdated)
    Q_PROPERTY(QString uptime READ uptime NOTIFY metricsUpdated)

public:
    explicit SystemMonitor(QObject *parent = nullptr);

    double cpuUsage() const;
    double memoryUsage() const;
    QString uptime() const;

private slots:
    void updateMetrics();

signals:
    void metricsUpdated();

private:
    bool readCpuCounters(quint64 &total, quint64 &idle);
    double readMemoryUsage();
    QString readUptime();

    QTimer m_timer;

    double m_cpuUsage = 0.0;
    double m_memoryUsage = 0.0;
    QString m_uptime = "--";

    quint64 m_previousTotal = 0;
    quint64 m_previousIdle = 0;
    bool m_hasPreviousCpuReading = false;
};

#endif
