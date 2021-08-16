/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ndn-greetings-app.hpp"
#include "static-routing-helper.hpp"
#include "wifi-adhoc-helper.hpp"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

namespace ns3 {

uint32_t MacTxDropCount, PhyTxDropCount, PhyRxDropCount;

void
MacTxDrop(Ptr<const Packet> p)
{
  MacTxDropCount++;
}

void
PhyTxDrop(Ptr<const Packet> p)
{
  PhyTxDropCount++;
}
void
PhyRxDrop(Ptr<const Packet> p, WifiPhyRxfailureReason r)
{
  PhyRxDropCount++;
}

void
PrintDrop()
{
  std::cout << Simulator::Now().GetSeconds() << "\t PktDropStats MacTxDrop=" << MacTxDropCount << "\t PhyTxDrop="<< PhyTxDropCount << "\t PhyRxDrop=" << PhyRxDropCount << "\n";
}


int
main(int argc, char* argv[])
{
  std::string phyMode("DsssRate1Mbps");
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
  int numNodes = 10;
  int range = 60;
  std::string traceFile;
  bool tracing = false;
  int duration = 60;

  CommandLine cmd;
  cmd.AddValue("numNodes", "numNodes", numNodes);
  cmd.AddValue("wifiRange", "the wifi range", range);
  cmd.AddValue("traceFile", "Ns2 movement trace file", traceFile);
  cmd.AddValue("tracing", "enable wifi tracing", tracing);
  cmd.Parse(argc, argv);

  if (traceFile.empty()) {
    std::cout << "Error: --traceFile is required" << std::endl;
    cmd.Usage("Test to see where this goes");
    exit(1);
  }

  ////////////////////////////////
  // Wi-Fi begin
  ////////////////////////////////
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue(phyMode),
                               "ControlMode", StringValue(phyMode));

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel",
                                  "MaxRange", DoubleValue(range));

  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel (wifiChannel.Create ());

  WifiMacHelper wifiMacHelper;
  wifiMacHelper.SetType("ns3::AdhocWifiMac");
  ////////////////////////////////
  // Wi-Fi end
  ////////////////////////////////

  NodeContainer nodes;
  nodes.Create (numNodes);

  NetDeviceContainer wifiNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, nodes);

  // Mobility model
  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
  ns2.Install ();

  // tracing
  if (tracing == true) {
    AsciiTraceHelper ascii;
    wifiPhyHelper.EnableAsciiAll (ascii.CreateFileStream ("wifi-tracing.tr"));
  }


  // Install NDN stack
  ndn::StackHelper ndnHelper;
  ndnHelper.AddFaceCreateCallback(WifiNetDevice::GetTypeId(), MakeCallback(FixLinkTypeAdhocCb));
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.Install(nodes);

  // Add route static routes to Application prefix
  std::string ndnGreetingsPrefix = "/localhop/ndn-greetings";
  AddStaticRouteAllNodes(ndnGreetingsPrefix+"/hi-i-am");

  // Set Forwarding Strategy
  ndn::StrategyChoiceHelper::Install(nodes, "/", "/localhost/nfd/strategy/multicast");

  // Install NDN Apps
  uint64_t idx = 0;
  for (const auto& node : NodeContainer::GetGlobal()) {
    std::string nodeName = "/ufba/Node"+std::to_string(idx);

    ndn::AppHelper appHelper("NdnGreetingsApp");
    appHelper.SetAttribute("AppPrefix", StringValue(ndnGreetingsPrefix));
    appHelper.SetAttribute("NodeName", StringValue(nodeName));
    appHelper.Install(node).Start(MilliSeconds(100+10*idx));

    idx += 1;
  }

  // Trace Collisions
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeCallback(&MacTxDrop));
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(&PhyTxDrop));
  Simulator::Schedule(Seconds(duration), &PrintDrop);

  Simulator::Stop(Seconds(duration));

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
