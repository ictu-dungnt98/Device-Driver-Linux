//
// Created by tainv on 08/08/2019.
//

#include <Controller/HcDefs.hpp>
#include <Libraries/Log/LogPlus.hpp>
#include <Libraries/Timer/SysTemTimer.hpp>
#include <Controller/JsonMessage/JsonKalive/JsonKeepAlive.hpp>
#include <Libraries/Utils/HardwareInfo.hpp>
#include <Libraries/JsonConfigure/JsConfig.hpp>
#include <Controller/JsonMessage/JsonIO/JsonIOSet.hpp>
#include <Controller/JsonMessage/JsonMsg/JsonErrorStatus.hpp>
#include "LocalSession.hpp"

LocalSession::LocalSession(
    String hc_mac,
    bool_t debug
) : hc_mac_(std::move(hc_mac)),
    is_master_(FALSE),
    debug_mode_(debug),
    local_secret_(JsConfig::GetInstance()->GetValue("local_secret").asString()),
    component_("localhost", LOCAL_PORT_COMPONENT, USERNAME_COMPONENT, PASSWORD_COMPONENT, 60, FALSE),
    client_("localhost", LOCAL_PORT_CLIENT, USERNAME_CLIENT, PASSWORD_CLIENT, 60, TRUE, HardwareInfo::GetSourcePath() + CAFILE_LOCAL),
    component_event_functor_(makeFunctor((Func1<MqttClient::MqttEvent>*) nullptr, *this, &LocalSession::HandleComponentEvent)),
    client_event_functor_(makeFunctor((Func1<MqttClient::MqttEvent>*) nullptr, *this, &LocalSession::HandleClientEvent)),
    component_functor_(makeFunctor((Func3<String, String, int_t>*) nullptr, *this, &LocalSession::ComponentMessageToJsonCommand)),
    client_functor_(makeFunctor((Func3<String, String, int_t>*) nullptr, *this, &LocalSession::ClientMessageToJsonCommand)),
    keepalive_functor_(makeFunctor((Func1R<void_p, void_p>*) nullptr, *this, &LocalSession::KeepAliveProcess))
{
    component_.AddFunctor(component_functor_);
    component_.AddFunctor(component_event_functor_);
    component_.SignalError() += Slot(this, &LocalSession::SendError);
    client_.AddFunctor(client_functor_);
    client_.AddFunctor(client_event_functor_);
    client_.SignalError() += Slot(this, &LocalSession::SendError);
    keepalive_thread_.AddFunctor(keepalive_functor_);
    Phone::getInstance();
}

void LocalSession::StartComponent() {
    Vector<String> component_topics = {
            LOCAL_TOPIC_ZIGBEE + "/+",
            LOCAL_TOPIC_ZWAVE + "/+",
            LOCAL_TOPIC_BLUETOOTH + "/+",
            LOCAL_TOPIC_NVR + "/+",
            LOCAL_TOPIC_OTA + "/+",
            LOCAL_TOPIC_SERIAL + "/+",
            LOCAL_TOPIC_IO + "/" + JSCMD_TOPIC_CONFIG,
            LOCAL_TOPIC_IO + "/" + JSCMD_TOPIC_CONTROL,
            LOCAL_TOPIC_IO + "/" + JSCMD_TOPIC_STATUS,
            LOCAL_TOPIC_IO + "/" + JSCMD_TOPIC_ERROR,
            LOCAL_TOPIC_POWER + "/+",
            LOCAL_TOPIC_TOOLTEST + "/" + JSCMD_TOPIC_CONFIG
    };
    component_.Start("core_process", component_topics);
    keepalive_thread_.Start();
}

void LocalSession::StartClient(bool_t is_master) {
    is_master_ = is_master;
    if (is_master_) {
        Vector<String> client_topics = {
            LOCAL_TOPIC_SLAVE + "/#",
            LOCAL_TOPIC_MOBILE + "/" + local_secret_ + "/+"
        };
        client_.Start("master_" + hc_mac_, client_topics);
    }
}

void LocalSession::ComponentMessageToJsonCommand(String topic, const String& message, int_t) {
    try {
        size_t position = 0, level = 0;
        String source, client_id;
        while ((position = topic.find('/')) != String::npos) {
            if (level == 1) {
                source = topic.substr(0, position);
                std::transform(source.begin(), source.end(), source.begin(), ::toupper);
            }

            topic.erase(0, position + 1);
            level++;
        }

        // Set source flag
        JsonCommand::Flag source_flag;
        if (source == "ZWAVE") {
            source_flag = JsonCommand::Flag::ZWAVE;
        } else if (source == "ZIGBEE") {
            source_flag = JsonCommand::Flag::ZIGBEE;
        } else if (source == "BLUETOOTH") {
            source_flag = JsonCommand::Flag::BLUETOOTH;
        } else if (source == "NVR") {
            source_flag = JsonCommand::Flag::NVR;
        } else if (source == "OTA") {
            source_flag = JsonCommand::Flag::OTA;
        } else if (source == "DSC") {
            source_flag = JsonCommand::Flag::SERIAL;
        } else if (source == "IO") {
            source_flag = JsonCommand::Flag::IO;
        } else if (source == "POWER") {
            source_flag = JsonCommand::Flag::POWER;
        } else if (source == "TOOLTEST") {
            source_flag = JsonCommand::Flag::TOOLTEST;
        } else {
            ThrowExceptionMessage(ErrorCode::ErrValue, "Invalid component source");
        }

        Json::Value parsed_message;
        Json::Reader reader;

        require_action(reader.parse(message, parsed_message, FALSE) && parsed_message.isObject(), ErrorCode::ErrFormat);
        assert_action(!(parsed_message.isMember("source") && parsed_message["source"].asString() == "core"));
        require_action(parsed_message.isMember("cmd") && parsed_message.isMember("reqid") &&
                       parsed_message.isMember("objects") && parsed_message["objects"].isArray(),
                       ErrorCode::ErrUnsupportedData);

        LOG_INFO("%s <-- %s: %s", topic.c_str(), source.c_str(), message.c_str());

        String cmd = parsed_message["cmd"].asString();
        String reqid = parsed_message["reqid"].asString();
        for (Json::Value &object : parsed_message["objects"]) {
            if (object.isMember("type") && object.isMember("data") && object["data"].isArray()) {
                String type = object["type"].asString();
                Json::Value data = object["data"];

                Json::FastWriter writer;
                String str_data = writer.write(data);
                str_data.erase(std::remove(str_data.begin(), str_data.end(), '\n'), str_data.end());

                JsonCommand_t jsonCommand(topic, cmd, type, data, source_flag, JsonCommand::Flag::COORD, reqid);
                jsonCommand.SetClientId(client_id);

                // Get version if exist
                if (object.isMember("versions")) {
                    jsonCommand.SetVersion(object["versions"].asInt());
                }
                if (object.isMember("serverversion")) {
                    jsonCommand.SetVersion(object["serverversion"].asInt());
                }
                if (object.isMember("execution")) {
                    jsonCommand.SetExecution(object["execution"]);
                }
                if (object.isMember("bridge_key")) {
                    jsonCommand.SetFromLocalBridge(object["bridge_key"].asString());
                }
                if (parsed_message.isMember("source")) {
                    jsonCommand.SetSource(parsed_message["source"].asString());
                }
                if (parsed_message.isMember("control_source") && parsed_message["control_source"].isMember("type") &&
                    parsed_message["control_source"].isMember("id") &&
                    parsed_message["control_source"].isMember("previous_control_reqid")) {
                    JsonCommand::ControlSource control_source(
                            JsonCommand::ConvertToControlSourceType[parsed_message["control_source"]["type"].asString()],
                            parsed_message["control_source"]["id"].asString(),
                            parsed_message["control_source"]["previous_control_reqid"].asString());
                    jsonCommand.SetControlSource(control_source);
                }

                signal_push_.Emit(jsonCommand);
            }
        }
    } catch (Exception2 &ex) {
        LOG_ERRO(ex.what());
        ex.setPayload(message);
        JsonCommand error = JsonErrorStatus::CreateJsonCommand(ex);
        error.SetSourceFlag(JsonCommand::Flag::COORD);
        error.SetDestinationFlag(JsonCommand::Flag::NETWORK);
        signal_push_.Emit(error);
    } catch (std::exception& ex) {
        Exception2 ex2 = ExceptionMessage(ErrorCode::ErrInternal, ex.what());
        LOG_ERRO(ex2.what());
        ex2.setPayload(message);
        JsonCommand error = JsonErrorStatus::CreateJsonCommand(ex2);
        error.SetSourceFlag(JsonCommand::Flag::COORD);
        error.SetDestinationFlag(JsonCommand::Flag::NETWORK);
        signal_push_.Emit(error);
    }
}

void LocalSession::ClientMessageToJsonCommand(String topic, const String& message, int_t) {
    try {
        size_t position = 0, level = 0;
        String source, client_id;
        while ((position = topic.find('/')) != String::npos) {
            if (level == 1) {
                source = topic.substr(0, position);
                std::transform(source.begin(), source.end(), source.begin(), ::toupper);
            } else if (level == 2) {
                client_id = topic.substr(0, position);
            }

            topic.erase(0, position + 1);
            level++;
        }

        // Set source flag
        JsonCommand::Flag source_flag;
        if (source == "SLAVE") {
            require_action_string(client_id != JsConfig::GetInstance()->GetValue("hc_id").asString(),
                                  ErrorCode::ErrValue, "Invalid Slave");
            source_flag = JsonCommand::Flag::LOCAL;
        } else if (source == "MOBILE") {
            require_action_string(client_id == local_secret_, ErrorCode::ErrValue, "Invalid local_secret");
            source_flag = JsonCommand::Flag::MOBILE;
        } else {
            ThrowExceptionMessage(ErrorCode::ErrValue, "Invalid client source");
        }

        Json::Value parsed_message;
        Json::Reader reader;

        require_action(reader.parse(message, parsed_message, FALSE) && parsed_message.isObject(), ErrorCode::ErrFormat);
        assert_action(!(parsed_message.isMember("source") && parsed_message["source"].asString() == hc_mac_));
        require_action(parsed_message.isMember("cmd") && parsed_message.isMember("reqid") &&
                       parsed_message.isMember("objects") && parsed_message["objects"].isArray(),
                       ErrorCode::ErrUnsupportedData);

        LOG_INFO("%s <-- %s%s: %s", topic.c_str(), source.c_str(),
                 source == "SLAVE" ? String("_" + client_id).c_str() : "", message.c_str());

        String cmd = parsed_message["cmd"].asString();
        String reqid = parsed_message["reqid"].asString();
        for (Json::Value &object : parsed_message["objects"]) {
            if (object.isMember("type") && object.isMember("data") && object["data"].isArray()) {
                // Control IO
                if (source == "MOBILE") {
                    JsonCommand_t io = JsonIOSet::CreateJsonCommand(JsonIOSet::EventCode::SERVER_RECEIVED_MESSAGE);
                    io.SetSourceFlag(JsonCommand::Flag::COORD);
                    io.SetDestinationFlag(JsonCommand::Flag::IO);
                    signal_push_.Emit(io);
                }

                // Get message
                String type = object["type"].asString();
                Json::Value data = object["data"];

                Json::FastWriter writer;
                String str_data = writer.write(data);
                str_data.erase(std::remove(str_data.begin(), str_data.end(), '\n'), str_data.end());

                JsonCommand_t jsonCommand(topic, cmd, type, data, source_flag, JsonCommand::Flag::COORD, reqid);
                jsonCommand.SetClientId(client_id);

                // Get version if exist
                if (object.isMember("versions")) {
                    jsonCommand.SetVersion(object["versions"].asInt());
                }
                if (object.isMember("serverversion")) {
                    jsonCommand.SetVersion(object["serverversion"].asInt());
                }
                if (object.isMember("execution")) {
                    jsonCommand.SetExecution(object["execution"]);
                }
                if (object.isMember("bridge_key")) {
                    jsonCommand.SetFromLocalBridge(object["bridge_key"].asString());
                }
                if (parsed_message.isMember("source")) {
                    jsonCommand.SetSource(parsed_message["source"].asString());
                }
                if (parsed_message.isMember("control_source") && parsed_message["control_source"].isMember("type") &&
                    parsed_message["control_source"].isMember("id") &&
                    parsed_message["control_source"].isMember("previous_control_reqid")) {
                    JsonCommand::ControlSource control_source(
                            JsonCommand::ConvertToControlSourceType[parsed_message["control_source"]["type"].asString()],
                            parsed_message["control_source"]["id"].asString(),
                            parsed_message["control_source"]["previous_control_reqid"].asString());
                    jsonCommand.SetControlSource(control_source);
                }

                signal_push_.Emit(jsonCommand);
            }
        }
    } catch (Exception2 &ex) {
        LOG_ERRO(ex.what());
        ex.setPayload(message);
        JsonCommand error = JsonErrorStatus::CreateJsonCommand(ex);
        error.SetSourceFlag(JsonCommand::Flag::COORD);
        error.SetDestinationFlag(JsonCommand::Flag::NETWORK);
        signal_push_.Emit(error);
    } catch (std::exception& ex) {
        Exception2 ex2 = ExceptionMessage(ErrorCode::ErrInternal, ex.what());
        LOG_ERRO(ex2.what());
        ex2.setPayload(message);
        JsonCommand error = JsonErrorStatus::CreateJsonCommand(ex2);
        error.SetSourceFlag(JsonCommand::Flag::COORD);
        error.SetDestinationFlag(JsonCommand::Flag::NETWORK);
        signal_push_.Emit(error);
    }
}

bool_t LocalSession::SendToComponent(JsonCommand &jsonCommand) {
    String topic = jsonCommand.GetTopic();
    if (!topic.empty()) {
        String full_topic, destination;
        if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::ZWAVE) {
            full_topic = LOCAL_TOPIC_ZWAVE + "/" + topic; destination = "ZWAVE";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::ZIGBEE) {
            full_topic = LOCAL_TOPIC_ZIGBEE + "/" + topic; destination = "ZIGBBE";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::SERIAL) {
            full_topic = LOCAL_TOPIC_SERIAL + "/" + topic; destination = "DSC";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::BLUETOOTH) {
            full_topic = LOCAL_TOPIC_BLUETOOTH + "/" + topic; destination = "BLUETOOTH";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::NVR) {
            full_topic = LOCAL_TOPIC_NVR + "/" + topic; destination = "NVR";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::OTA) {
            full_topic = LOCAL_TOPIC_OTA + "/" + topic; destination = "OTA";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::IO) {
            full_topic = LOCAL_TOPIC_IO + "/" + topic; destination = "IO";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::POWER) {
            full_topic = LOCAL_TOPIC_POWER + "/" + topic; destination = "POWER";
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::TOOLTEST) {
            full_topic = LOCAL_TOPIC_TOOLTEST + "/" + topic; destination = "TOOLTEST";
        } else {
            LOG_ERRO("Invalid destination flag");
            return FALSE;
        }

        jsonCommand.SetSource("core");

        LOG_INFO("%s --> %s: %s", topic.c_str(), destination.c_str(), jsonCommand.GetFullStringCommand().c_str());
        return component_.Publish(full_topic, jsonCommand.GetFullStringCommand());
    } else {
        LOG_ERRO("Topic not found");
        return FALSE;
    }
}

bool_t LocalSession::SendToClient(JsonCommand &jsonCommand) {
    String topic = jsonCommand.GetTopic();
    if (!topic.empty()) {
        String full_topic;
        if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::LOCAL) {
            full_topic = LOCAL_TOPIC_SLAVE + "/" + jsonCommand.GetClientId() + "/" + topic;
            LOG_INFO("%s --> SLAVE_%s: %s", topic.c_str(), jsonCommand.GetClientId().c_str(), jsonCommand.GetFullStringCommand().c_str());
        } else if (jsonCommand.GetDestinationFlag() == JsonCommand::Flag::MOBILE || jsonCommand.GetDestinationFlag() == JsonCommand::Flag::NETWORK) {
            full_topic = LOCAL_TOPIC_MOBILE + "/" + local_secret_ + "/" + topic;
            LOG_INFO("%s --> MOBILE: %s", topic.c_str(), jsonCommand.GetFullStringCommand().c_str());
        } else {
            LOG_ERRO("Invalid destination flag");
            return FALSE;
        }

        jsonCommand.SetSource(hc_mac_);
        return client_.Publish(full_topic, jsonCommand.GetFullStringCommand());
    } else {
        LOG_ERRO("Topic not found");
        return FALSE;
    }
}

bool_t LocalSession::SendToPhone(JsonCommand &jsonCommand) {
    LOG_INFO("%s --> PHONE: %s", JsonCommand::MapJsonCommandFlag[jsonCommand.GetSourceFlag()].c_str(), jsonCommand.GetFullStringCommand().c_str());
    Json::Value jsValue = jsonCommand.GetData();
    String message;
    if (jsValue.isArray() && !jsValue.empty()) {
        for (Json::Value &it :  jsValue) {
            message = it["content"].asString();
            if (!it.isMember("smsDcom") || !it.isMember("callDcom")) {
                return FALSE;
            }
            for (auto &phone_number : it["smsDcom"]) {
                if (phone_number.empty()) {
                    return TRUE;
                }
                String smsPhone = "Sms_" + phone_number.asString() + String("_") + message;
                LOG_DBUG("%s", smsPhone.c_str());
                Phone::getInstance()->AddWork(smsPhone);
            }
            for (auto &phone_number : it["callDcom"]) {
                if (phone_number.empty()) {
                    return TRUE;
                }
                String region_code = "84";
                String convert_phone_number = phone_number.asString();
                convert_phone_number.erase(std::remove(convert_phone_number.begin(), convert_phone_number.end(), '+'), convert_phone_number.end());
                String tmp = convert_phone_number.substr(0,2);
                if (tmp == region_code) {
                    convert_phone_number.replace(convert_phone_number.find(region_code), region_code.length(),"0");
                }
                LOG_DBUG("phone %s", convert_phone_number.c_str());
                String callCommand = "Call_" + convert_phone_number;
                Phone::getInstance()->AddWork(callCommand);
            }
        }
    }
    return TRUE;
}

bool_t LocalSession::SubscribeSlave(String &client_id) {
    return client_.Subscribe(LOCAL_TOPIC_SLAVE + "/" + client_id + "/+");
}

bool_t LocalSession::SubscribeMobile(String &client_id) {
    return client_.Subscribe(LOCAL_TOPIC_MOBILE + "/" + client_id + "/+");
}

bool_t LocalSession::IsConnectComponent() {
    return component_.IsConnect();
}

bool_t LocalSession::IsConnectClient() {
    return client_.IsConnect();
}

void LocalSession::AddFunctor(Func1<JsonCommand &> functor) {
    signal_push_.Attach(functor);
}

void LocalSession::DelFunctor(Func1<JsonCommand &> functor) {
    signal_push_.Detach(functor);
}

void LocalSession::SetDebugMode(bool_t mode) {
    debug_mode_ = mode;
}

void LocalSession::SendError(int_t error) {
    signal_error_.Emit(error);
}

void LocalSession::HandleComponentEvent(MqttClient::MqttEvent event) {
    if (event == MqttClient::MQTT_EVENT_CONNECT) {
        LOG_WARN("Connected to component");
    }
}

void LocalSession::HandleClientEvent(MqttClient::MqttEvent event) {
    if (event == MqttClient::MQTT_EVENT_CONNECT) {
        LOG_WARN("Connected to client");
    }
}

void_p LocalSession::KeepAliveProcess(void_p) {
    while (keepalive_thread_.IsRunning()) {
        LOG_DBUG("Keep alive with process manager");
        JsonCommand_t jsonSend = JsonKeepAlive::CreateJsonCommand();
        component_.Publish("component/keepalive/" + HardwareInfo::GetProgramName(), jsonSend.GetFullStringCommand());
        sleep(60);
    }
    return nullptr;
}