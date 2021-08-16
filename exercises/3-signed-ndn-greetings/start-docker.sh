#!/bin/bash

SCRIPT=$(realpath $0)
MYDIR=$(dirname $SCRIPT)
NAME=ndn-tutorial-e3

docker rm -f $NAME >/dev/null 2>&1

docker run -d --name $NAME \
  -v $MYDIR:/simulation \
  -w /simulation \
  -u $(id -u):$(id -g) \
  -e PYTHONPATH=/home/ndn/ndnSIM/ns-3/src/visualizer:/home/ndn/ndnSIM/ns-3/build/bindings/python \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --ipc=host \
  --cap-drop=ALL \
  --security-opt=no-new-privileges \
  emrevoid/ndnsim:2.8-debug tail -f /dev/null

docker cp ../../misc/fix-visualizer.patch $NAME:/tmp/
docker exec -it $NAME bash -c 'cd /home/ndn/ndnSIM/ns-3/src/visualizer && git apply /tmp/fix-visualizer.patch'
