/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef HELLOWORLD_PRODUCER_HPP
#define HELLOWORLD_PRODUCER_HPP

#include <string>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/util/time.hpp>

namespace ndn {
namespace helloworld {

class HelloworldProducer
{
public:
  HelloworldProducer(Name prefix);
  void run();
  void cleanup();
  void Start();
  void Stop();

private:
  void OnHelloworldInterest(const ndn::Interest& interest);

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
  Name m_prefix;
};

} // namespace helloworld
} // namespace ndn

#endif // HELLOWORLD_PRODUCER_HPP
