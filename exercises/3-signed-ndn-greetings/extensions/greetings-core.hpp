/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef GREETINGS_HPP
#define GREETINGS_HPP

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

class Greetings
{
public:
  Greetings();
  std::string NextGreetings();

private:
  std::random_device rdevice_;
  std::mt19937 m_rengine;

  std::vector<std::string> m_greetings = {
    "How are you doing?",
    "How’s it going?",
    "How have you been?",
    "What's up?",
    "What’s new?",
    "What’s going on?",
    "How’s everything?",
    "How are things?",
    "How’s life?",
    "How’s your day?",
    "How’s your day going?",
    "Good to see you!",
    "Nice to see you",
    "Long time no see",
    "It’s been a while",
    "Pleased to see you",
    "Are you OK?",
    "You alright?",
    "Whazzup?",
    "Sup?"
  };
};

} // namespace greetings
} // namespace ndn

#endif // GREETINGS_HPP
