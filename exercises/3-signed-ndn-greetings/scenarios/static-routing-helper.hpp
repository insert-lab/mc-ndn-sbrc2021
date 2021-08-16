#ifndef _STATIC_ROUTING_HELPER_HPP_
#define _STATIC_ROUTING_HELPER_HPP_

#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"

inline void
AddStaticRouteAllNodes(std::string prefix) {
  using namespace ns3;

  for (const auto& node : NodeContainer::GetGlobal()) {
    for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
      Ptr<NetDevice> device = node->GetDevice(deviceId);
      Ptr<ns3::ndn::L3Protocol> ndn = node->GetObject<ns3::ndn::L3Protocol>();
      if (ndn==0)
        continue;

      shared_ptr<ns3::ndn::Face> face = ndn->getFaceByNetDevice(device);
      if (face==0)
        continue;

      //std::cout << "*****************face id = " << face->getId() << std::endl;
      //std::cout << "*****************face info: " << *face << std::endl;
      //std::cout << "*****************face linkType: " << face->getLinkType() << std::endl;
      //std::cout << "*****************face Transport: " << face->getTransport() << std::endl;
      ns3::ndn::FibHelper::AddRoute(node, prefix, face->getId(), 0);
    }
  }
}

#endif  // _STATIC_ROUTING_HELPER_HPP_
