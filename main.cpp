#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <iostream>

class HttpServer : public QTcpServer {
    Q_OBJECT

public:
    HttpServer(QObject *parent = nullptr) : QTcpServer(parent) {
        if (!this->listen(QHostAddress::Any, 80)) {
            std::cerr << "Failed to start server: " << this->errorString().toStdString() << std::endl;
        } else {
            std::cout << "Server is running on port 80." << std::endl;
        }
    }

public slots:  // Changed this to public slots
    void newConnection() {
        QTcpSocket *socket = this->nextPendingConnection();

        connect(socket, &QTcpSocket::readyRead, [this, socket]() {
            while (socket->canReadLine()) {
                QString line = QString::fromUtf8(socket->readLine()).trimmed();
                if (line.isEmpty()) {
                    break; // End of headers
                }
                std::cout << line.toStdString() << std::endl; // Optional: Print the request
            }

            // Prepare a simple HTTP response
            QByteArray response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/plain\r\n"
                                  "Connection: close\r\n"
                                  "\r\n"
                                  "PONG";


            // Send the response
            socket->write(response);
            socket->flush();
            socket->disconnectFromHost();
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    HttpServer server;

    QObject::connect(&server, &QTcpServer::newConnection, &server, &HttpServer::newConnection);

    return app.exec();
}

#include "main.moc"
