#!/bin/bash

NAME=ndn-tutorial-e3

docker exec -it -e NS_LOG=ndn.Greetings $NAME ./waf --run "ndn-greetings-grid-p2p --nNodes=2"
