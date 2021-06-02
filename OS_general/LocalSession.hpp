//
// Created by tainv on 08/08/2019.
//

#ifndef HC_LOCALSESSION_HPP
#define HC_LOCALSESSION_HPP

#include "Libraries/Utils/Core.hpp"
#include "Libraries/Utils/Typedefs.h"
#include "Libraries/Utils/Vector.hpp"
#include "Libraries/Utils/String.hpp"
#include "Libraries/Utils/Signal.hpp"
#include "Libraries/Signal/SignalF.hpp"
#include "Libraries/Functors/Functor.hpp"
#include "Libraries/Utils/SharedPtr.hpp"
#include "Drivers/Net/MQTT/MqttClient.hpp"
#include "Drivers/Phone/Phone.hpp"
#include "Session/Net/JsonCommand.hpp"

class LocalSession : public Core {
public:
    LocalSession(String hc_mac, bool_t debug);
    ~LocalSession() override = default;

    void StartComponent();
    void StartClient(bool_t is_master);

    void ComponentMessageToJsonCommand(String topic, const String& message, int_t length);
    void ClientMessageToJsonCommand(String topic, const String& message, int_t length);
    bool_t SendToComponent(JsonCommand& jsonCommand);
    bool_t SendToClient(JsonCommand& jsonCommand);
    static bool_t SendToPhone(JsonCommand& jsonCommand);
    bool_t SubscribeSlave(String& client_id);
    bool_t SubscribeMobile(String& client_id);

    bool_t IsConnectComponent();
    bool_t IsConnectClient();

    void AddFunctor(Func1<JsonCommand&> functor);
    void DelFunctor(Func1<JsonCommand&> functor);

    void SetDebugMode(bool_t mode);

private:
    MqttClient_t component_;
    MqttClient_t client_;

    // Signals and Functors
    SigFunc1<JsonCommand&> signal_push_;
    Signal<void(int_t)> signal_error_;
    Func3<String, String, int_t> component_functor_;
    Func3<String, String, int_t> client_functor_;
    Func1<MqttClient::MqttEvent> component_event_functor_;
    Func1<MqttClient::MqttEvent> client_event_functor_;

    // Information
    String hc_mac_;
    bool_t is_master_;
    bool_t debug_mode_;
    String local_secret_;

    // Timer
    Thread_t keepalive_thread_;
    Func1R<void_p, void_p> keepalive_functor_;

    // Processes
    void_p KeepAliveProcess(void_p buffer);

    void SendError(int_t error);
    void HandleComponentEvent(MqttClient::MqttEvent event);
    void HandleClientEvent(MqttClient::MqttEvent event);

    DISABLE_COPY(LocalSession);
};

typedef LocalSession  LocalSession_t;
typedef LocalSession* LocalSession_p;

#endif //HC_LOCALSESSION_HPP
