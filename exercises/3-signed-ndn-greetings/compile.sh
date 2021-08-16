#!/bin/bash

docker exec -it ndn-tutorial-e3 ./waf configure --debug
docker exec -it ndn-tutorial-e3 ./waf
