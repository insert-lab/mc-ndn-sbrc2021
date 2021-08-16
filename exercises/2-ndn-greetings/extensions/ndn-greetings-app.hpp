/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef NDNGREETINGS_APP_HPP
#define NDNGREETINGS_APP_HPP

#include "ndn-greetings.hpp"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"
#include <ns3/core-module.h>

namespace ns3 {

// Class inheriting from ns3::Application
class NdnGreetingsApp : public Application
{
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("NdnGreetingsApp")
      .SetParent<Application>()
      .AddConstructor<NdnGreetingsApp>()
      .AddAttribute("AppPrefix", "NDN Greetings Application Prefix", StringValue("/localhop/ndn-greetings"),
                    MakeStringAccessor(&NdnGreetingsApp::_appPrefix), MakeStringChecker())
      .AddAttribute("NodeName", "Node name", StringValue("/ndn/NodeA"),
                    MakeStringAccessor(&NdnGreetingsApp::_nodeName), MakeStringChecker());

    return tid;
  }

protected:
  virtual void StartApplication() {
    m_instance.reset(new ::ndn::greetings::NdnGreetings(_appPrefix, _nodeName));
    m_instance->Start();
  }

  virtual void StopApplication() {
    m_instance->Stop();
    m_instance.reset();
  }

private:
  std::unique_ptr<::ndn::greetings::NdnGreetings> m_instance;
  std::string _appPrefix;
  std::string _nodeName;
};

} // namespace ns3

#endif // NDNGREETINGS_APP_HPP
