/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef NDNGREETINGS_HPP
#define NDNGREETINGS_HPP

#include "greetings-core.hpp"

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <random>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time.hpp>

namespace ndn {
namespace greetings {

static const std::string kHelloType = "hi-i-am";
static const std::string kGreetingsType = "greetings";

class NdnGreetings
{
public:
  NdnGreetings(Name appPrefix, Name nodeName);
  void run();
  void cleanup();
  void Start();
  void Stop();

private:
  void SendHelloInterest();
  void OnHelloInterest(const ndn::Interest& interest);
  void OnGreetingsInterest(const ndn::Interest& interest);
  void ReplyGreetingsInterest(const ndn::Interest& interest);
  void OnGreetingsContent(const ndn::Interest& interest, const ndn::Data& data);
  void OnGreetingsTimedOut(const ndn::Interest& interest);
  void OnGreetingsNack(const ndn::Interest& interest, const ndn::lp::Nack& nack);
  void SendGreetingsInterest(const std::string& neighName);

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
  ndn::Scheduler m_scheduler;
  Name m_appPrefix;
  Name m_nodeName;
  std::random_device rdevice_;
  std::mt19937 m_rengine;
  std::uniform_int_distribution<int> m_rand_nonce;

  std::map<std::string, std::string> m_neighMap;

  Greetings m_greetings;
};

} // namespace greetings
} // namespace ndn

#endif // NDNGREETINGS_HPP
