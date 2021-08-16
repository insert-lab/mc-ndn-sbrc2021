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
  , m_keyChain("pib-memory:", "tpm-memory:")
  , m_validator(m_face)
  , m_appPrefix(appPrefix)
  , m_nodeName(nodeName)
  , m_rengine(rdevice_())
  , m_rand_nonce(0, std::numeric_limits<int>::max())
{
  // TODO 1: Loading the Data Validator Config

  /* Register the HELLO application prefix in the FIB */
  Name appHelloPrefix = Name(m_appPrefix);
  appHelloPrefix.append(kHelloType);
  m_face.setInterestFilter(appHelloPrefix, std::bind(&NdnGreetings::OnHelloInterest, this, _2),
    [this](const Name&, const std::string& reason) {
      throw std::runtime_error("Failed to register sync interest prefix: " + reason);
  });

  /* Register the GREETINGS application prefix in the FIB */
  Name appGreetingsPrefix = Name(m_appPrefix);
  appGreetingsPrefix.append(kGreetingsType);
  appGreetingsPrefix.append(m_nodeName);
  m_face.setInterestFilter(appGreetingsPrefix, std::bind(&NdnGreetings::OnGreetingsInterest, this, _2),
    [this](const Name&, const std::string& reason) {
      throw std::runtime_error("Failed to register sync interest prefix: " + reason);
  });

  // TODO 3: Set Interest Filter for KEY (m_nodeName + KEY)
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

  // XXX: Sending periodic hello interest
  // m_scheduler.schedule(time::seconds(1), [this] { SendHelloInterest(); });
}

void NdnGreetings::OnHelloInterest(const ndn::Interest& interest) {
  /* Sanity check to avoid hello interest from myself */
  std::string neighName = interest.getName().getSubName(m_appPrefix.size()+1).toUri();
  if (neighName == m_nodeName) {
    return;
  }
  MYLOG_INFO("Received HELLO Interest " << interest.getName());

  auto neigh = m_neighMap.find(neighName);
  if (neigh == m_neighMap.end()) {
    SendGreetingsInterest(neighName);
  } else {
    MYLOG_INFO("Skipping already known neighbor" << neighName);
  }
}

void
NdnGreetings::SendGreetingsInterest(const std::string& neighName) {
  MYLOG_INFO("Sending greetings Interest to neighbor=" << neighName);
  Name name = Name(m_appPrefix);
  name.append(kGreetingsType);
  name.append(neighName);

  Interest interest = Interest();
  interest.setNonce(m_rand_nonce(m_rengine));
  interest.setName(name);
  interest.setCanBePrefix(false);
  interest.setMustBeFresh(true);
  interest.setInterestLifetime(time::seconds(5));

  m_face.expressInterest(interest,
    std::bind(&NdnGreetings::OnGreetingsContent, this, _1, _2),
    std::bind(&NdnGreetings::OnGreetingsNack, this, _1, _2),
    std::bind(&NdnGreetings::OnGreetingsTimedOut, this, _1));
}

void NdnGreetings::OnGreetingsInterest(const ndn::Interest& interest) {
  MYLOG_INFO("Received greetings Interest " << interest.getName());

  auto data = std::make_shared<ndn::Data>(interest.getName());
  data->setFreshnessPeriod(ndn::time::milliseconds(1000));

  // Set greetings answer
  std::string greetings_str = m_greetings.NextGreetings();
  data->setContent(reinterpret_cast<const uint8_t*>(greetings_str.c_str()), greetings_str.size());
  m_keyChain.sign(*data, m_signingInfo);
  m_face.put(*data);

  MYLOG_INFO("Greetings sent! MyMsg: " << greetings_str);
}

void NdnGreetings::OnGreetingsContent(const ndn::Interest& interest, const ndn::Data& data) {
  // TODO 2: Call the Validator upon receiving a data packet
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

void NdnGreetings::OnGreetingsValidated(const ndn::Data& data) {
  MYLOG_DEBUG("Validated data: " << data.getName());

  std::string neighName = data.getName().getSubName(m_appPrefix.size()+1).toUri();

  /* Get data content */
  size_t content_size = data.getContent().value_size();
  std::string content_value((char *)data.getContent().value(), content_size);
  MYLOG_DEBUG("Received Greetings from=" << neighName << " size=" << content_size << " msg=" << content_value);

  m_neighMap.emplace(neighName, content_value);
}

void NdnGreetings::OnGreetingsValidationFailed(const ndn::Data& data, const ndn::security::v2::ValidationError& ve) {
  MYLOG_DEBUG("Fail to validate data: name: " << data.getName() << " error: " << ve);
}

ndn::security::v2::Certificate 
NdnGreetings::GetCertificate() {
  try {
    /* cleanup: remove any possible existing certificate with the same name */
    m_keyChain.deleteIdentity(m_keyChain.getPib().getIdentity(m_nodeName));
  }
  catch (const std::exception& e) {
  }
  ndn::security::Identity myIdentity  = m_keyChain.createIdentity(m_nodeName);
  ndn::security::v2::Certificate myCert = myIdentity.getDefaultKey().getDefaultCertificate();

  ndn::Name myCertName = myCert.getKeyName();
  myCertName.append("NA");
  myCertName.appendVersion();
  myCert.setName(myCertName);

  return myCert;
}

void 
NdnGreetings::InstallSignedCertificate(ndn::security::v2::Certificate signedCert) {
  auto myIdentity = m_keyChain.getPib().getIdentity(m_nodeName);
  auto myKey = myIdentity.getKey(signedCert.getKeyName());
  m_keyChain.addCertificate(myKey, signedCert);
  m_keyChain.setDefaultCertificate(myKey, signedCert);
  m_signingInfo = ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, m_nodeName);
}

void
NdnGreetings::OnKeyInterest(const ndn::Interest& interest) {
  // TODO 4: reply KEY interest with node's Key
}

} // namespace greetings
} // namespace ndn
