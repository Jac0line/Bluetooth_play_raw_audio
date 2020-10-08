#include "ui_chat.h"

#include <QtWidgets/qdialog.h>

#include <QtBluetooth/qbluetoothhostinfo.h>
#include <QtBluetooth/qbluetoothuuid.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QAudio>
#include <QtMultimedia/QAudioOutput>
#include <QFile>

QT_USE_NAMESPACE

/**
 * @brief The Chat class Create and maintain a QBluetoothSocket connection
 */
class Chat : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Chat Constructs an Chat instance
     * @param parent pointer to the QWidget address
     */
    explicit Chat(QWidget *parent = nullptr);

    ~Chat();

signals:
    void messageReceived(const QString &sender, const QString &message);
    void connected(const QString &name);
    void disconnected();
    void socketErrorOccurred(const QString &errorString);

private slots:
    void connectClicked();
    void quitClicked();
    void playClicked();
    void clientDisconnected();
    void startClient(const QBluetoothServiceInfo &remoteService, const QBluetoothAddress localDevice);
    void stopClient();
    void readSocket();
    void connected();
    void onSocketErrorOccurred(QBluetoothSocket::SocketError);
    void stateChanged(QAudio::State);

private:
    int currentAdapterIndex = 0;
    static const QBluetoothUuid::ServiceClassUuid uuid = QBluetoothUuid::AudioSink;
    Ui_Chat *ui;

    QList<Chat *> clients;
    QList<QBluetoothHostInfo> localAdapters;

    QString localName;
    QBluetoothSocket *socket = nullptr;
    QAudioOutput* m_audioOutput;
    QFile sourceFile;
};
