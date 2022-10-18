#!/bin/bash

docker run -it --name ubuntu20.04 -v /home/dungnt98:/home/dungnt98 --rm dungnt98-ubuntu20.04 >/dev/null

if [[ ! $? -eq 0 ]]; then
        docker attach dungnt98-ubuntu20.04
fi
