
#include "SystemMonitor.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringList>

#include <algorithm>

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &SystemMonitor::updateMetrics);

    updateMetrics();

    m_timer.start(1000);
}

double SystemMonitor::cpuUsage() const
{
    return m_cpuUsage;
}

double SystemMonitor::memoryUsage() const
{
    return m_memoryUsage;
}

QString SystemMonitor::uptime() const
{
    return m_uptime;
}

bool SystemMonitor::readCpuCounters(quint64 &total, quint64 &idle)
{
    QFile file("/proc/stat");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    const QString line = in.readLine();

    const QStringList fields =
        line.split(' ', Qt::SkipEmptyParts);

    if (fields.size() < 5 || fields[0] != "cpu")
        return false;

    total = 0;

    for (int i = 1; i < fields.size(); ++i)
        total += fields[i].toULongLong();

    const quint64 idleTime = fields[4].toULongLong();

    const quint64 ioWait =
        fields.size() > 5 ? fields[5].toULongLong() : 0;

    idle = idleTime + ioWait;

    return true;
}

double SystemMonitor::readMemoryUsage()
{
    QFile file("/proc/meminfo");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0.0;

    const QString contents = QString::fromLatin1(file.readAll());

    const QRegularExpression totalRegex(
        QStringLiteral("MemTotal:\\s+(\\d+)")
    );

    const QRegularExpression availableRegex(
        QStringLiteral("MemAvailable:\\s+(\\d+)")
    );

    const auto totalMatch = totalRegex.match(contents);
    const auto availableMatch = availableRegex.match(contents);

    if (!totalMatch.hasMatch() || !availableMatch.hasMatch())
        return 0.0;

    bool totalOk = false;
    bool availableOk = false;

    const quint64 totalMemory =
        totalMatch.captured(1).toULongLong(&totalOk);

    const quint64 availableMemory =
        availableMatch.captured(1).toULongLong(&availableOk);

    if (!totalOk || !availableOk || totalMemory == 0)
        return 0.0;

    const quint64 usedMemory =
        totalMemory - qMin(availableMemory, totalMemory);

    const double usage =
        100.0 * static_cast<double>(usedMemory)
        / static_cast<double>(totalMemory);

    return usage;
}

QString SystemMonitor::readUptime()
{
    QFile file("/proc/uptime");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "--";

    QTextStream in(&file);

    double seconds = 0.0;
    in >> seconds;

    const int days = static_cast<int>(seconds / 86400);
    const int hours = static_cast<int>(seconds / 3600) % 24;
    const int minutes = static_cast<int>(seconds / 60) % 60;

    return QString("%1d %2h %3m")
        .arg(days)
        .arg(hours)
        .arg(minutes);
}

void SystemMonitor::updateMetrics()
{
    quint64 total = 0;
    quint64 idle = 0;

    if (readCpuCounters(total, idle)) {
        if (m_hasPreviousCpuReading) {
            const quint64 totalDelta = total - m_previousTotal;
            const quint64 idleDelta = idle - m_previousIdle;

            if (totalDelta > 0) {
                m_cpuUsage =
                    100.0 * (totalDelta - idleDelta) / totalDelta;

                m_cpuUsage =
                    std::clamp(m_cpuUsage, 0.0, 100.0);
            }
        }

        m_previousTotal = total;
        m_previousIdle = idle;
        m_hasPreviousCpuReading = true;
    }

    m_memoryUsage = readMemoryUsage();
    m_uptime = readUptime();

    emit metricsUpdated();
}
