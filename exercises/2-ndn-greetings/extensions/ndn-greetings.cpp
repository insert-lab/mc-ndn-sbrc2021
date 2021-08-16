/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ndn-greetings.hpp"

#include <limits>
#include <cmath>
#include <boost/algorithm/string.hpp> 
#include <algorithm>

#define MYLOG_COMPONENT "ndn.Greetings"
#include "mylog-helper.hpp"

namespace ndn {
namespace greetings {

NdnGreetings::NdnGreetings(Name appPrefix, Name nodeName)
  : m_scheduler(m_face.getIoService())
  , m_appPrefix(appPrefix)
  , m_nodeName(nodeName)
  , m_rengine(rdevice_())
  , m_rand_nonce(0, std::numeric_limits<int>::max())
{
  // TODO 1: Set Interest Filter for HELLO prefix (m_appPrefix + kHelloType)

  // TODO 4: Set Interest Filter for Greetings prefix (m_appPrefix + kGreetingsType)
}

void NdnGreetings::Start() {
  m_scheduler.schedule(time::seconds(1), [this] { SendHelloInterest(); });
}

void NdnGreetings::Stop() {
}

void NdnGreetings::run() {
  m_face.processEvents();
}

void NdnGreetings::cleanup() {
}

void
NdnGreetings::SendHelloInterest() {
  Name name = Name(m_appPrefix);
  name.append(kHelloType);
  name.append(m_nodeName);
  MYLOG_INFO("Sending Interest " << name);

  Interest interest = Interest();
  interest.setNonce(m_rand_nonce(m_rengine));
  interest.setName(name);
  interest.setCanBePrefix(false);
  interest.setInterestLifetime(time::milliseconds(1));

  m_face.expressInterest(interest, [](const Interest&, const Data&) {},
                        [](const Interest&, const lp::Nack&) {},
                        [](const Interest&) {});

  // TODO 6: Sending periodic hello interest
  //m_scheduler.schedule(time::seconds(1), [this] { SendHelloInterest(); });
}

void NdnGreetings::OnHelloInterest(const ndn::Interest& interest) {
  // TODO 2: extract neighbor name; if neighbor is new, send Greetings Interest
}

void
NdnGreetings::SendGreetingsInterest(const std::string& neighName) {
  // TODO 3: Sending Greetings Interest upon discovering nodes
}

void NdnGreetings::OnGreetingsInterest(const ndn::Interest& interest) {
  // TODO 5: Reply to Greetings interest
}

void NdnGreetings::OnGreetingsContent(const ndn::Interest& interest, const ndn::Data& data) {
  std::string neighName = data.getName().getSubName(m_appPrefix.size()+1).toUri();

  /* Security validation */
  //if (data.getSignature().hasKeyLocator()) {
  //  MYLOG_DEBUG("Data signed with: " << data.getSignature().getKeyLocator().getName());
  //}

  /* Get data content */
  size_t content_size = data.getContent().value_size();
  std::string content_value((char *)data.getContent().value(), content_size);
  MYLOG_DEBUG("Received Greetings from=" << neighName << " size=" << content_size << " msg=" << content_value);

  m_neighMap.emplace(neighName, content_value);
}

void NdnGreetings::OnGreetingsTimedOut(const ndn::Interest& interest) {
  MYLOG_DEBUG("Interest timed out for Name: " << interest.getName());

  /* should we retransmit? How many times? */
  //std::string neighName = interestName.getSubName(m_appPrefix.size()+1).toUri();
  //SendGreetingsInterest(neighName);
}

void NdnGreetings::OnGreetingsNack(const ndn::Interest& interest, const ndn::lp::Nack& nack) {
  MYLOG_DEBUG("Received Nack with reason: " << nack.getReason());
}

} // namespace greetings
} // namespace ndn
