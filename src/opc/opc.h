//
// Created by AC on 2024/3/13.
//

#pragma once

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

    void request_endpoints();

    void read_node();

    void write_attribute(const QJsonArray &json_array);

    void update_nodes(const QJsonArray &json_array);


public slots:

    void on_endpoints_request_finished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode,
                                       QUrl requestUrl);

    void on_state_changed(QOpcUaClient::ClientState state);

    void on_attribute_read(QOpcUa::NodeAttributes attributes);

private:
    QOpcUaClient *client_ = nullptr;
    QOpcUaProvider *provider_ = nullptr;
    QMap<const QString &, QOpcUaNode *> nodes_;
};


