#include "client_manager.h"

ClientManager::ClientManager()
{

}

void ClientManager::clear()
{
    _clients.clear();
}

void ClientManager::addClient(const ShPtrDeviceClient& client)
{
    _clients.insert(client);

    connect(client.get(), &DeviceClient::commandSuccessed,
            this, &ClientManager::onCommandSuccessed);
    client->setLiceningStreamPort(PORT + static_cast<quint16>(_clients.size()));
}

void ClientManager::removeClient(const ShPtrDeviceClient& client)
{
    _clients.erase(client);
    disconnect(client.get(), &DeviceClient::commandSuccessed,
               this, &ClientManager::onCommandSuccessed);
}

void ClientManager::startClients()
{
    for(auto& client : _clients)
    {
        client->start();
    }
}

void ClientManager::stopClients()
{
    for (auto& client : _clients)
    {
        client->stop();
    }
}

void ClientManager::setCommand(proto::receiver::Command& command)
{
    if(_commandQueue.isEmpty())
    {
        sendCommandForAll(command);
    }

    _commandQueue.enqueue(command);
}

void ClientManager::sendCommandForAll( proto::receiver::Command& command)
{
    for(auto& client : _clients)
    {
        client->sendCommand(command);
        _counter++;
    }
}

void ClientManager::onCommandSuccessed()
{
    if(_commandQueue.isEmpty())
    {
        return ;
    }
    if(--_counter != 0)
    {
        return ;
    }
    const proto::receiver::Command& successedCommand = _commandQueue.dequeue();

    if(successedCommand.command_type() == proto::receiver::START_DDC1)
    {
        std::vector<ShPtrPacketBuffer> buffers;
        for(auto& client : _clients)
        {
            buffers.push_back(client->getDDC1Buffer());
        }
        emit ready(buffers);
    }
    else if(successedCommand.command_type() == proto::receiver::STOP_DDC1)
    {
        emit notReady();
    }

    if(!_commandQueue.isEmpty() &&
            _commandQueue.head().command_type() != successedCommand.command_type())
    {
        sendCommandForAll(_commandQueue.head());
    }
}

//void DeviceSetListWidget::checkingConnections(bool checkingState)
//{
//    std::shared_ptr<QTimer> shpTimer = std::make_shared<QTimer>();
//    std::shared_ptr<QTime> shpTime = std::make_shared<QTime>();
//    std::shared_ptr<QMetaObject::Connection>shpConnection =
//        std::make_shared<QMetaObject::Connection>();
//    shpTime->start();

//    *shpConnection = connect(shpTimer.get(), &QTimer::timeout,
//                             [this, checkingState, shpTimer, shpTime, shpConnection]
//    {
//        bool success = false;

//        for(const auto& device : _deviceses)
//        {
//            if(DeviceSetWidget* deviceSetWidget =
//                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(device.first)))
//            {
//                if(checkingState == deviceSetWidget->isConnected())
//                {
//                    success = true;
//                }
//                else
//                {
//                    success = false;
//                    break;
//                }
//            }
//        }

//        if(!success && shpTime->elapsed() >= TIME_WAIT_CONNECTION)
//        {
//            QObject::disconnect(*shpConnection);
//            setCursor(Qt::ArrowCursor);
//            QMessageBox::warning(this, "Connection Failed", "");
//        }
//        else if(success)
//        {
//            QObject::disconnect(*shpConnection);
//            setCursor(Qt::ArrowCursor);
//        }
//    });

//    shpTimer->start(500);
//}
