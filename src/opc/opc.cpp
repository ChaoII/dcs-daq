//
// Created by AC on 2024/3/13.
//

#include "opc/opc.h"
#include "utils/config.h"
#include "utils/utils.h"
#include<QThread>
#include <QRandomGenerator>
#include <QOpcUaErrorState>

OPC::OPC() {
    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    provider_ = new QOpcUaProvider(this);
    client_ = provider_->createClient(QString("open62541"));
    client_->requestEndpoints(QUrl(Config::Instance().OPC_server_endpoint));
    client_->setNamespaceAutoupdate(true);
    connect(client_, &QOpcUaClient::endpointsRequestFinished, this, &OPC::on_endpoints_request_finished);
    connect(client_, &QOpcUaClient::stateChanged, this, &OPC::on_state_changed);
    connect(timer_, &QTimer::timeout, this, &OPC::on_reconnect_endpoint);
    timer_->start();
}

void OPC::on_endpoints_request_finished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode,
                                        QUrl requestUrl) {
    if (!endpoints.empty()) {
        client_->connectToEndpoint(endpoints[0]); // Connect to the first endpoint in the list
    }
}

void OPC::on_state_changed(QOpcUaClient::ClientState state) {
    if (state == QOpcUaClient::ClientState::Connected) {
        update_nodes_map();
        qDebug() << "+++++++++++connect successful";
    } else if (state == QOpcUaClient::ClientState::Disconnected) {
        qDebug() << "+++++++++++++connect failed, try to reconnect";
    }
}

void OPC::write_attribute(const QJsonArray &json_array) {
    if (client_->state() == QOpcUaClient::ClientState::Connected) {
        for (auto &json: json_array) {
            auto node_id = json.toObject().value("tag_id").toString();
            auto node = node_map_.value(node_id);
            QVariant value = json.toObject().value("text").toDouble();
            if (node) {
                // node->writeAttribute(QOpcUa::NodeAttribute::Value, value, QOpcUa::Types::Float);
                node->writeAttribute(QOpcUa::NodeAttribute::Value, QRandomGenerator::global()->bounded(0, 50),
                                     QOpcUa::Types::Float);
            }
        }
    }
}

OPC::~OPC() {

}

void OPC::update_nodes_map() {
    if (client_->state() == QOpcUaClient::ClientState::Connected) {
        for (auto json: json_array_node_) {
            auto node_id = json.toObject().value("tag_id").toString();
            auto node = client_->node(Config::Instance().OPC_prefix + node_id);
            node_map_[node_id] = node;
        }
    }
}

void OPC::update_nodes_array(const QJsonArray &json_array) {
    json_array_node_ = json_array;
    update_nodes_map();
}

void OPC::on_reconnect_endpoint() {
    if (client_->state() != QOpcUaClient::ClientState::Connected) {
        client_->requestEndpoints(QUrl(Config::Instance().OPC_server_endpoint));
    }
}
