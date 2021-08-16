/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "greetings-core.hpp"

namespace ndn {
namespace greetings {

Greetings::Greetings()
  : m_rengine(rdevice_())
{
}

std::string
Greetings::NextGreetings() {
  std::uniform_int_distribution<int> m_rand_idx(0, m_greetings.size()-1);
  int idx = m_rand_idx(m_rengine);
  return m_greetings[idx];
}
} // namespace greetings
} // namespace ndn
