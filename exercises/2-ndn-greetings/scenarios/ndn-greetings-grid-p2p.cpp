/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ndn-greetings-app.hpp"
#include "static-routing-helper.hpp"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 * This scenario simulates a grid topology (using PointToPointGrid module)
 *
 */

int
main(int argc, char* argv[])
{
  int nNodes = 3;
  // Setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("0ms"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.AddValue("nNodes", "N nodes to create a NxN grid topology", nNodes);
  cmd.Parse(argc, argv);

  // Creating NxN topology
  PointToPointHelper p2p;
  PointToPointGridHelper grid(nNodes, nNodes, p2p);
  grid.BoundingBox(100, 100, 400, 400);

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Add route static routes to Application prefix
  std::string ndnGreetingsPrefix = "/localhop/ndn-greetings";
  AddStaticRouteAllNodes(ndnGreetingsPrefix+"/hi-i-am");

  // Set Multicast strategy
  ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/multicast");

  // Install NDN Apps
  uint64_t idx = 0;
  for (const auto& node : NodeContainer::GetGlobal()) {
    std::string nodeName = "/ufba/Node"+std::to_string(idx);

    ndn::AppHelper appHelper("NdnGreetingsApp");
    appHelper.SetAttribute("AppPrefix", StringValue(ndnGreetingsPrefix));
    appHelper.SetAttribute("NodeName", StringValue(nodeName));
    appHelper.Install(node).Start(MilliSeconds(10));

    idx += 1;
  }

  Simulator::Stop(Seconds(30.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
