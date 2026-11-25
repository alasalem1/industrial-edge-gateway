#ifndef FAULTMANAGER_H
#define FAULTMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QString>
#include <QJsonArray>
#include <QSet>

class FaultManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList events READ events NOTIFY eventsChanged)
    Q_PROPERTY(int activeFaultCount READ activeFaultCount NOTIFY eventsChanged)
    Q_PROPERTY(int unacknowledgedCount READ unacknowledgedCount NOTIFY eventsChanged)
    Q_PROPERTY(QString storageStatus READ storageStatus NOTIFY storageStatusChanged)

public:
    explicit FaultManager(QObject *parent = nullptr);

    QVariantList events() const;
    int activeFaultCount() const;
    int unacknowledgedCount() const;
    QString storageStatus() const;

    Q_INVOKABLE bool acknowledge(const QString &eventId);
    Q_INVOKABLE void acknowledgeAll();

public slots:
    void processTelemetry(bool connected,
                          int faultMask,
                          const QString &severity,
                          const QString &summary);

signals:
    void eventsChanged();
    void storageStatusChanged();

private:
    void loadHistory();
    bool saveHistory();
    void addFaultEvent(int faultCode,
                       const QString &title,
                       const QString &severity,
                       const QString &summary);
    void clearFaultEvent(int faultCode);
    void setStorageStatus(const QString &status);
    int countActive() const;
    int countUnacknowledged() const;

    QJsonArray m_history;
    QString m_filePath;
    QString m_storageStatus = "Initializing";
    QSet<int> m_knownFaults;
    bool m_firstValidSample = true;

    static constexpr int MaxEvents = 200;
};

#endif
