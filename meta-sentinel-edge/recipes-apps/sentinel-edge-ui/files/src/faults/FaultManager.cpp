#include "FaultManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QUuid>
#include <QVariantMap>

namespace {
struct FaultDescription {
    int code;
    const char *title;
};

const FaultDescription faultDescriptions[] = {
    {1, "High temperature"},
    {2, "High pressure"},
    {4, "High vibration"},
    {8, "High motor speed"}
};
}

FaultManager::FaultManager(QObject *parent)
    : QObject(parent)
{
    m_filePath = qEnvironmentVariable(
        "SENTINEL_EDGE_HISTORY_FILE",
        "/var/lib/sentinel-edge/fault-history.json");

    loadHistory();
}

QVariantList FaultManager::events() const
{
    return m_history.toVariantList();
}

int FaultManager::countActive() const
{
    int count = 0;

    for (const QJsonValue &value : m_history) {
        if (value.toObject().value("active").toBool())
            ++count;
    }

    return count;
}

int FaultManager::countUnacknowledged() const
{
    int count = 0;

    for (const QJsonValue &value : m_history) {
        const QJsonObject event = value.toObject();

        if (event.value("active").toBool() &&
            !event.value("acknowledged").toBool()) {
            ++count;
        }
    }

    return count;
}

int FaultManager::activeFaultCount() const
{
    return countActive();
}

int FaultManager::unacknowledgedCount() const
{
    return countUnacknowledged();
}

QString FaultManager::storageStatus() const
{
    return m_storageStatus;
}

void FaultManager::setStorageStatus(const QString &status)
{
    if (m_storageStatus == status)
        return;

    m_storageStatus = status;
    emit storageStatusChanged();
}

void FaultManager::loadHistory()
{
    QFile file(m_filePath);

    if (!file.exists()) {
        setStorageStatus("New history file");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        setStorageStatus("History read failed");
        return;
    }

    QJsonParseError error;
    const QJsonDocument document =
        QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError ||
        !document.isArray()) {
        setStorageStatus("History file is invalid");
        return;
    }

    m_history = document.array();

    while (m_history.size() > MaxEvents)
        m_history.removeLast();

    setStorageStatus("History loaded");
}

bool FaultManager::saveHistory()
{
    const QFileInfo info(m_filePath);
    QDir directory;

    if (!directory.mkpath(info.absolutePath())) {
        setStorageStatus("Cannot create history directory");
        return false;
    }

    QSaveFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        setStorageStatus("History write failed");
        return false;
    }

    const QByteArray data =
        QJsonDocument(m_history).toJson(QJsonDocument::Indented);

    if (file.write(data) != data.size()) {
        file.cancelWriting();
        setStorageStatus("History write incomplete");
        return false;
    }

    if (!file.commit()) {
        setStorageStatus("History commit failed");
        return false;
    }

    setStorageStatus("History saved");
    return true;
}

void FaultManager::addFaultEvent(int faultCode,
                                const QString &title,
                                const QString &severity,
                                const QString &summary)
{
    QJsonObject event;

    event.insert("id", QUuid::createUuid().toString(QUuid::WithoutBraces));
    event.insert("timestamp",
                 QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    event.insert("faultCode", faultCode);
    event.insert("title", title);
    event.insert("severity", severity);
    event.insert("message", summary);
    event.insert("active", true);
    event.insert("acknowledged", false);
    event.insert("state", "ACTIVE");

    m_history.prepend(event);

    while (m_history.size() > MaxEvents)
        m_history.removeLast();
}

void FaultManager::clearFaultEvent(int faultCode)
{
    for (int i = 0; i < m_history.size(); ++i) {
        QJsonObject event = m_history.at(i).toObject();

        if (event.value("faultCode").toInt() != faultCode ||
            !event.value("active").toBool()) {
            continue;
        }

        event.insert("active", false);
        event.insert("state", "CLEARED");
        event.insert("clearedAt",
                     QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

        m_history.replace(i, event);
    }
}

void FaultManager::processTelemetry(bool connected,
                                    int faultMask,
                                    const QString &severity,
                                    const QString &summary)
{
    // Missing or stale telemetry is not proof that a machine fault cleared.
    if (!connected)
        return;

    bool changed = false;

    for (const FaultDescription &fault : faultDescriptions) {
        const bool currentlyPresent = (faultMask & fault.code) != 0;
        bool existingActiveEvent = false;

        for (const QJsonValue &value : m_history) {
            const QJsonObject event = value.toObject();

            if (event.value("faultCode").toInt() == fault.code &&
                event.value("active").toBool()) {
                existingActiveEvent = true;
                break;
            }
        }

        if (currentlyPresent && !existingActiveEvent) {
            addFaultEvent(fault.code,
                          QString::fromLatin1(fault.title),
                          severity,
                          summary);
            changed = true;
        } else if (!currentlyPresent && existingActiveEvent) {
            clearFaultEvent(fault.code);
            changed = true;
        }
    }

    m_firstValidSample = false;

    if (changed) {
        saveHistory();
        emit eventsChanged();
    }
}

bool FaultManager::acknowledge(const QString &eventId)
{
    if (eventId.trimmed().isEmpty())
        return false;

    bool changed = false;

    for (int i = 0; i < m_history.size(); ++i) {
        QJsonObject event = m_history.at(i).toObject();

        if (event.value("id").toString() != eventId ||
            !event.value("active").toBool() ||
            event.value("acknowledged").toBool()) {
            continue;
        }

        event.insert("acknowledged", true);
        event.insert("state", "ACKNOWLEDGED");
        event.insert("acknowledgedAt",
                     QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

        m_history.replace(i, event);
        changed = true;
        break;
    }

    if (!changed)
        return false;

    saveHistory();
    emit eventsChanged();
    return true;
}

void FaultManager::acknowledgeAll()
{
    bool changed = false;

    for (int i = 0; i < m_history.size(); ++i) {
        QJsonObject event = m_history.at(i).toObject();

        if (!event.value("active").toBool() ||
            event.value("acknowledged").toBool()) {
            continue;
        }

        event.insert("acknowledged", true);
        event.insert("state", "ACKNOWLEDGED");
        event.insert("acknowledgedAt",
                     QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

        m_history.replace(i, event);
        changed = true;
    }

    if (changed) {
        saveHistory();
        emit eventsChanged();
    }
}
