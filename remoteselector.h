#ifndef REMOTESELECTOR_H
#define REMOTESELECTOR_H

#include <QtWidgets/qdialog.h>

#include <QtBluetooth/qbluetoothaddress.h>
#include <QtBluetooth/qbluetoothserviceinfo.h>
#include <QtBluetooth/qbluetoothuuid.h>

QT_FORWARD_DECLARE_CLASS(QBluetoothServiceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui {
    class RemoteSelector;
}
QT_END_NAMESPACE


/**
 * @brief The RemoteSelector class Constructs a remote selector with the mac address of the local bluetooth device (dongle)
 * provides functionality to discover other bluetooth devices.
 */
class RemoteSelector : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief RemoteSelector Constructs a remote selector
     * @param localAdapter The mac adress of the local Bluetooth adapter (dongle)
     * @param parent A pointer to the QWidget parent
     */
    explicit RemoteSelector(const QBluetoothAddress &localAdapter, QWidget *parent = nullptr);

    ~RemoteSelector();

    /**
     * @brief startDiscovery Starts a QBluetoothServiceDiscoveryAgent
     * @param uuid Filter only devices which offer the uuid service
     */
    void startDiscovery(const QBluetoothUuid &uuid);

    /**
     * @brief stopDiscovery Stop the QBluetoothServiceDiscoveryAgent
     */
    void stopDiscovery();

    /**
     * @brief service Returns the QBluetoothServiceInfo of a discovered device
     * @return The QBluetoothServiceInfo of a discovered device
     */
    QBluetoothServiceInfo service() const;

private:
    Ui::RemoteSelector *ui;

    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QBluetoothServiceInfo m_service;
    QMap<QListWidgetItem *, QBluetoothServiceInfo> m_discoveredServices;

private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void discoveryFinished();
    void on_remoteDevices_itemActivated(QListWidgetItem *item);
    void on_cancelButton_clicked();
};

#endif // REMOTESELECTOR_H
