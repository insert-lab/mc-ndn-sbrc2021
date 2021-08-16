#!/bin/bash

docker exec -e NS_LOG=ndn.Greetings -it ndn-tutorial-e2 ./waf --run "ndn-greetings-mobility --numNodes=10 --traceFile=solution/scenario-10nodes-RandomWalk-300x300.ns_movements" $@
