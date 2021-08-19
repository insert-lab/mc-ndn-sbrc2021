#!/bin/bash

docker exec -it -e NS_LOG=ndn.HelloworldConsumer:ndn.HelloworldProducer ndn-tutorial-e1 ./waf --run "helloworld-2nodes" $@
