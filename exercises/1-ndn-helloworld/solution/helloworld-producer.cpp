/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "helloworld-producer.hpp"

#include <limits>
#include <cmath>
#include <boost/algorithm/string.hpp> 
#include <algorithm>

#include <ns3/log.h>
NS_LOG_COMPONENT_DEFINE("ndn.HelloworldProducer");

namespace ndn {
namespace helloworld {

HelloworldProducer::HelloworldProducer(Name prefix)
  : m_prefix(prefix)
{
  m_face.setInterestFilter(m_prefix, std::bind(&HelloworldProducer::OnHelloworldInterest, this, _2),
    [this](const Name&, const std::string& reason) {
      throw std::runtime_error("Failed to register sync interest prefix: " + reason);
  });
}

void HelloworldProducer::Start() {
}

void HelloworldProducer::Stop() {
}

void HelloworldProducer::run() {
  m_face.processEvents();
}

void HelloworldProducer::cleanup() {
}

void HelloworldProducer::OnHelloworldInterest(const ndn::Interest& interest) {
  NS_LOG_INFO("Received helloworld Interest " << interest.getName());

  auto data = std::make_shared<ndn::Data>(interest.getName());
  data->setFreshnessPeriod(ndn::time::milliseconds(1000));

  std::string data_str = "Hello World!!!";
  data->setContent(reinterpret_cast<const uint8_t*>(data_str.c_str()), data_str.size());
  m_keyChain.sign(*data);
  m_face.put(*data);

  NS_LOG_INFO("Data sent! Content: " << data_str);
}
} // namespace helloworld
} // namespace ndn
