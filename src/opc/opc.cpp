//
// Created by AC on 2024/3/13.
//

#include "opc/opc.h"

OPC::OPC() {
    provider_ = new QOpcUaProvider(this);
    client_ = provider_->createClient(QString("open62541"));
    client_->requestEndpoints(QUrl("opc.tcp://127.0.0.1:49320"));
    connect(client_, &QOpcUaClient::endpointsRequestFinished, this, &OPC::on_endpoints_request_finished);
    connect(client_, &QOpcUaClient::stateChanged, this, &OPC::on_state_changed);
}

void OPC::on_endpoints_request_finished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode,
                                        QUrl requestUrl) {
    qDebug() << endpoints.at(0).endpointUrl();
    qDebug() << statusCode;
    qDebug() << "Endpoints returned:" << endpoints.count();
    qDebug() << endpoints.size();
    if (!endpoints.empty()) {
        client_->connectToEndpoint(endpoints[0]); // Connect to the first endpoint in the list
    }
}

void OPC::on_state_changed(QOpcUaClient::ClientState state) {
    qDebug() << "Client state changed:" << state;
    if (state == QOpcUaClient::ClientState::Connected) {
        //QOpcUaNode定义
        for (auto &node: nodes_) {
            node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(500));
            connect(node, &QOpcUaNode::attributeRead, this, &OPC::on_attribute_read);
            connect(node, &QOpcUaNode::attributeUpdated, this, &OPC::on_attribute_read);
        }
    }
}

void OPC::on_attribute_read(QOpcUa::NodeAttributes attributes) {
//    qDebug() << "Signal for attributes:" << attributes;
//    qDebug() << "value:" << node_->attribute(QOpcUa::NodeAttribute::Value).toBool();
//    qDebug() << "BrowseName:" << node_->attribute(QOpcUa::NodeAttribute::BrowseName);
//    qDebug() << "DataType:" << node_->attribute(QOpcUa::NodeAttribute::DataType);
}

void OPC::request_endpoints() {

}

void OPC::read_node() {
//    node_->readAttributes(
//            QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DataType);
//    qDebug() << "readnode" << node_->nodeId();
}

void OPC::write_attribute(const QJsonArray &json_array) {

    for (auto &json: json_array) {
        auto node_id = json.toObject().value("name").toString();
        nodes_.push_back(client_->node(node_id));
    }

    QVariant var = false;
    node_->writeAttribute(QOpcUa::NodeAttribute::Value, var);
}

OPC::~OPC() {

}

void OPC::update_nodes(const QJsonArray &json_array) {
    for (auto &json: json_array) {
        auto node_id = json.toObject().value("name").toString();
        nodes_.push_back(client_->node(node_id));
    }
}
