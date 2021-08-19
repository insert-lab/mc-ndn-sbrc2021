/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "helloworld-consumer.hpp"

#include <limits>
#include <cmath>
#include <boost/algorithm/string.hpp> 
#include <algorithm>

#include <ns3/log.h>
NS_LOG_COMPONENT_DEFINE("ndn.HelloworldConsumer");

namespace ndn {
namespace helloworld {

HelloworldConsumer::HelloworldConsumer(Name prefix)
  : m_scheduler(m_face.getIoService())
  , m_prefix(prefix)
  , m_seq(0)
  , m_rengine(rdevice_())
  , m_rand_nonce(0, std::numeric_limits<int>::max())
{
}

void HelloworldConsumer::Start() {
  m_scheduler.schedule(time::seconds(1), [this] { SendInterest(); });
}

void HelloworldConsumer::Stop() {
}

void HelloworldConsumer::run() {
  m_face.processEvents();
}

void HelloworldConsumer::cleanup() {
}

void
HelloworldConsumer::SendInterest() {
  Name name = Name(m_prefix);
  //name.appendNumber(m_seq++);
  NS_LOG_INFO("Sending Interest " << name);

  Interest interest = Interest();
  interest.setNonce(m_rand_nonce(m_rengine));
  interest.setName(name);
  interest.setCanBePrefix(false);
  interest.setInterestLifetime(time::seconds(1));

  m_face.expressInterest(interest,
    std::bind(&HelloworldConsumer::OnHelloworldContent, this, _1, _2),
    std::bind(&HelloworldConsumer::OnHelloworldNack, this, _1, _2),
    std::bind(&HelloworldConsumer::OnHelloworldTimedOut, this, _1));

  //m_scheduler.schedule(time::seconds(1), [this] { SendInterest(); });
}

void HelloworldConsumer::OnHelloworldContent(const ndn::Interest& interest, const ndn::Data& data) {
  NS_LOG_INFO("Received Helloworld " << data.getName());

  size_t data_size = data.getContent().value_size();
  std::string data_value((char *)data.getContent().value(), data_size);
  NS_LOG_INFO("Received data: size=" << data_size << " value=" << data_value);
}

void HelloworldConsumer::OnHelloworldTimedOut(const ndn::Interest& interest) {
  NS_LOG_INFO("Interest timed out for Name: " << interest.getName());
}

void HelloworldConsumer::OnHelloworldNack(const ndn::Interest& interest, const ndn::lp::Nack& nack) {
  NS_LOG_INFO("Received Nack with reason: " << nack.getReason());
}

} // namespace helloworld
} // namespace ndn
