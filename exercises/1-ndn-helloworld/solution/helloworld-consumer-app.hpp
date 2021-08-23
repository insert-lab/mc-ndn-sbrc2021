/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef HELLOWORLD_CONSUMER_APP_HPP
#define HELLOWORLD_CONSUMER_APP_HPP

#include "helloworld-consumer.hpp"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"
#include <ns3/core-module.h>

namespace ns3 {

// Class inheriting from ns3::Application
class HelloworldConsumerApp : public Application
{
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("HelloworldConsumerApp")
      .SetParent<Application>()
      .AddConstructor<HelloworldConsumerApp>()
      .AddAttribute("Prefix", "NDN Helloworld Prefix", StringValue("/ndn/helloworld"),
                    MakeStringAccessor(&HelloworldConsumerApp::_prefix), MakeStringChecker());

    return tid;
  }

protected:
  virtual void StartApplication() {
    m_instance.reset(new ::ndn::helloworld::HelloworldConsumer(_prefix));
    m_instance->Start();
  }

  virtual void StopApplication() {
    m_instance->Stop();
    m_instance.reset();
  }

private:
  std::unique_ptr<::ndn::helloworld::HelloworldConsumer> m_instance;
  std::string _prefix;
};

} // namespace ns3

#endif // HELLOWORLD_CONSUMER_APP_HPP
