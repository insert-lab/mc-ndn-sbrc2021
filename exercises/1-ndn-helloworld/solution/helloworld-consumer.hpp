/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef HELLOWORLD_CONSUMER_HPP
#define HELLOWORLD_CONSUMER_HPP

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
namespace helloworld {

class HelloworldConsumer
{
public:
  HelloworldConsumer(Name prefix);
  void run();
  void cleanup();
  void Start();
  void Stop();

private:
  void SendInterest();
  void OnHelloworldContent(const ndn::Interest& interest, const ndn::Data& data);
  void OnHelloworldTimedOut(const ndn::Interest& interest);
  void OnHelloworldNack(const ndn::Interest& interest, const ndn::lp::Nack& nack);

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
  ndn::Scheduler m_scheduler;
  Name m_prefix;
  uint32_t  m_seq;
  std::random_device rdevice_;
  std::mt19937 m_rengine;
  std::uniform_int_distribution<int> m_rand_nonce;
};

} // namespace helloworld
} // namespace ndn

#endif // HELLOWORLD_CONSUMER_HPP
