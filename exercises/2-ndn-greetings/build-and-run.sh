#!/bin/bash

docker exec -it -e NS_LOG=ndn.Greetings ndn-tutorial-e2 ./waf --run "ndn-greetings-grid-p2p --nNodes=2" $@
