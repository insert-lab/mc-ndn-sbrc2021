/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef HELLOWORLD_PRODUCER_APP_HPP
#define HELLOWORLD_PRODUCER_APP_HPP

#include "helloworld-producer.hpp"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"
#include <ns3/core-module.h>

namespace ns3 {

// Class inheriting from ns3::Application
class HelloworldProducerApp : public Application
{
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("HelloworldProducerApp")
      .SetParent<Application>()
      .AddConstructor<HelloworldProducerApp>()
      .AddAttribute("Prefix", "NDN Helloworld Prefix", StringValue("/ndn/helloworld"),
                    MakeStringAccessor(&HelloworldProducerApp::_prefix), MakeStringChecker());

    return tid;
  }

protected:
  virtual void StartApplication() {
    m_instance.reset(new ::ndn::helloworld::HelloworldProducer(_prefix));
    m_instance->Start();
  }

  virtual void StopApplication() {
    m_instance->Stop();
    m_instance.reset();
  }

private:
  std::unique_ptr<::ndn::helloworld::HelloworldProducer> m_instance;
  std::string _prefix;
};

} // namespace ns3

#endif // HELLOWORLD_PRODUCER_APP_HPP
