#include "chat.h"
#include "remoteselector.h"

#include <QtCore/qdebug.h>
#include <QtBluetooth/qbluetoothdeviceinfo.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QTime>
#include <QtCore/qmetaobject.h>

Chat::Chat(QWidget *parent)
    : QDialog(parent), ui(new Ui_Chat)
{
    ui->setupUi(this);

    connect(ui->quitButton, &QPushButton::clicked, this, &Chat::quitClicked);
    connect(ui->connectButton, &QPushButton::clicked, this, &Chat::connectClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &Chat::playClicked);

    localName = QBluetoothLocalDevice().name();
}

Chat::~Chat()
{
    qDeleteAll(clients);
}

void Chat::clientDisconnected()
{
    Chat *client = qobject_cast<Chat *>(sender());
    if (client) {
        clients.removeOne(client);
        client->deleteLater();
    }
}

void Chat::quitClicked()
{
    socket->disconnectFromService();
    socket->close();
}

void Chat::connectClicked()
{
    ui->connectButton->setEnabled(false);

    const QBluetoothAddress adapter = localAdapters.isEmpty() ?
                                           QBluetoothAddress() :
                                           localAdapters.at(currentAdapterIndex).address();
//    const QBluetoothAddress adapter = *new QBluetoothAddress(*new QString("08:BE:AC:11:F1:87"));

    RemoteSelector remoteSelector(adapter);
    remoteSelector.startDiscovery(uuid);
    if (remoteSelector.exec() == QDialog::Accepted) {
        QBluetoothServiceInfo service = remoteSelector.service();
        service.setServiceUuid(uuid);

        qDebug() << "Connecting to service" << service.serviceName()
                 << "on" << service.device().name();

        // Create client
        qDebug() << "Going to create client";
        Chat *client = new Chat(this);
        qDebug() << "Connecting...";

        connect(client, &Chat::disconnected,
                this, QOverload<>::of(&Chat::clientDisconnected));
        qDebug() << "Start client";
        client->startClient(service, adapter);

        clients.append(client);
    }
    ui->connectButton->setEnabled(true);
}

void Chat::playClicked()
{
    qDebug() << "playClicked";
    const QString &str = "bluez";
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();

    //list all output devices
    QList<QAudioDeviceInfo> availableDevices = device.availableDevices(QAudio::AudioOutput);
    qDebug() << "Available devices: ";
    for (const QAudioDeviceInfo &deviceInfo : availableDevices)
        qDebug() << "Device name: " << deviceInfo.deviceName();

    //if outputdevice is connect trough blueZ (bluetooth)
    if(QAudioDeviceInfo::defaultOutputDevice().deviceName().contains(str)){
        qDebug() << "defaulth = bluez";

        QAudioFormat format;
        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        qDebug() << "defaultOutputDevice: "<< device.defaultOutputDevice().deviceName();
        m_audioOutput = new QAudioOutput(format);
        connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),SLOT(stateChanged(QAudio::State)));
        sourceFile.setFileName("/home/jacoline/Music/audio.raw");
        sourceFile.open(QIODevice::ReadOnly);
        m_audioOutput->start(&sourceFile);
    }
    else{
        qDebug() << "No blueZ output devices";
        qDebug() << QAudioDeviceInfo::defaultOutputDevice().deviceName();
    }
}


void Chat::startClient(const QBluetoothServiceInfo &remoteService, const QBluetoothAddress localDevice)
{
    if (socket)
        return;

    // Connect to service
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    qDebug() << "Create socket";
    qDebug() << remoteService.serviceUuid();
    qDebug() << remoteService.serviceName();
    qDebug() << remoteService.socketProtocol();
    qDebug() << remoteService.serviceProvider();
    qDebug() << remoteService.serviceAvailability();
    qDebug() << remoteService.device().name();
    qDebug() << remoteService.device().address();
    qDebug() << localDevice;

    socket->connectToService(remoteService,QIODevice::ReadOnly);
    qDebug() << "ConnectToService done";

    QBluetoothSocket::SocketError error = socket->error();
    qDebug() << "SocketError: " << error;
    QString stringError = socket->errorString();
    qDebug() << "SocketErrorString: " << error;
    qDebug() << "openMode: " << socket->openMode();

    connect(socket, &QBluetoothSocket::readyRead, this, &Chat::readSocket);
    connect(socket, &QBluetoothSocket::connected, this, QOverload<>::of(&Chat::connected));
    connect(socket, &QBluetoothSocket::disconnected, this, &Chat::disconnected);
    connect(socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error),
            this, &Chat::onSocketErrorOccurred);

    //wait 5 sec to give time to connect socket
    QTime dieTime = QTime::currentTime().addMSecs(5000);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);
    }
    qDebug() << "socketState: " << socket->state();

}

void Chat::stopClient()
{
    delete socket;
    socket = nullptr;
}

void Chat::readSocket()
{
    if (!socket)
        return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine();
        emit messageReceived(socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));
    }
}

void Chat::onSocketErrorOccurred(QBluetoothSocket::SocketError error)
{
    if (error == QBluetoothSocket::NoSocketError)
        return;

    QMetaEnum metaEnum = QMetaEnum::fromType<QBluetoothSocket::SocketError>();
    QString errorString = socket->peerName() + QLatin1Char(' ') + metaEnum.valueToKey(error) + QLatin1String(" occurred");

    emit socketErrorOccurred(errorString);
}

void Chat::connected()
{
    qDebug() << "Connected";
    emit connected(socket->peerName());
}

void Chat::stateChanged(QAudio::State state)
{
    qDebug() << "State changed to: " << state;
    switch (state) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            m_audioOutput->stop();
            sourceFile.close();
            delete m_audioOutput;
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (m_audioOutput->error() != QAudio::NoError) {
                qDebug() << "QAudio state error" << m_audioOutput->error();
            }
            break;

        default:

            break;
    }
}
