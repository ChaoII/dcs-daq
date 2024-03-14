//
// Created by AC on 2024/3/13.
//

#include "opc/opc.h"
#include "config.h"
#include "utils/utils.h"

OPC::OPC() {
    provider_ = new QOpcUaProvider(this);
    client_ = provider_->createClient(QString("open62541"));
    client_->requestEndpoints(QUrl("opc.tcp://127.0.0.1:49320"));
    connect(client_, &QOpcUaClient::endpointsRequestFinished, this, &OPC::on_endpoints_request_finished);
    connect(client_, &QOpcUaClient::stateChanged, this, &OPC::on_state_changed);
}

void OPC::on_endpoints_request_finished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode,
                                        QUrl requestUrl) {
    if (!endpoints.empty()) {
        client_->connectToEndpoint(endpoints[0]); // Connect to the first endpoint in the list
    }
}

void OPC::on_state_changed(QOpcUaClient::ClientState state) {
    if (state == QOpcUaClient::ClientState::Connected) {
        qDebug() << "connect successful";
    } else if (state == QOpcUaClient::ClientState::Disconnected) {
        qDebug() << "connect failed, try to reconnect";
        client_->requestEndpoints(QUrl("opc.tcp://127.0.0.1:49320"));
    }
}

void OPC::write_attribute(const QJsonArray &json_array) {
    for (auto &json: json_array) {
        auto node_id = json.toObject().value("tag_id").toString();
        auto node = node_map_.value(node_id);
        QVariant value = json.toObject().value("text").toDouble();
        node->writeAttribute(QOpcUa::NodeAttribute::Value, value.toFloat());
    }
}

OPC::~OPC() {

}

void OPC::update_nodes(const QJsonArray &json_array) {
    for (auto &json: json_array) {
        auto node_id = json.toObject().value("tag_id").toString();
        auto node = client_->node(Config::OPC_prefix + node_id);
        node_map_[node_id] = node;
    }
}
