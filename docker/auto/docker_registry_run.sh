#!/bin/bash
docker run -d -p 5000:5000 \
-v /opt/data/docker/registry:/var/lib/registry/docker/registry/v2 \
registry
