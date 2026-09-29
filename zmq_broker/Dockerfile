# Build: docker build -t tiny-twin-zmq-broker ocudu/tiny-twin-test/zmq_broker
FROM ubuntu:24.04 AS build
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      cmake g++ ninja-build pkg-config libzmq3-dev && rm -rf /var/lib/apt/lists/*
COPY CMakeLists.txt zmq_broker.cpp /src/
RUN cmake -S /src -B /build -GNinja -DCMAKE_BUILD_TYPE=Release && ninja -C /build

FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libzmq5 \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /build/zmq_broker /usr/local/bin/zmq_broker
ENTRYPOINT ["/usr/local/bin/zmq_broker"]
