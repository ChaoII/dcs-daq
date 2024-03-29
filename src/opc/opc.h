//
// Created by AC on 2024/3/13.
//

#pragma once

#include <QTimer>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaProvider>


class OPC : public QObject {
Q_OBJECT
public:
    explicit OPC();

    ~OPC() override;

    void write_attribute(const QJsonArray &json_array);

    void update_nodes_array(const QJsonArray &json_array);

    void update_nodes_map();


public slots:

    void on_endpoints_request_finished(QList<QOpcUaEndpointDescription> endpoints,
                                       QOpcUa::UaStatusCode statusCode,
                                       QUrl requestUrl);

    void on_state_changed(QOpcUaClient::ClientState state);

    void on_reconnect_endpoint();


private:
    QTimer *timer_ = nullptr;
    QOpcUaClient *client_ = nullptr;
    QOpcUaProvider *provider_ = nullptr;
    QMap<QString, QOpcUaNode *> node_map_;
    QJsonArray json_array_node_;
};


