FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
  build-essential autoconf git pkg-config \
  automake libtool curl make g++ unzip libcurl4-openssl-dev \
  && apt-get clean
RUN apt install -y cmake

# install or-tools first, then grpc
RUN git clone -b main https://github.com/google/or-tools

RUN cd or-tools && cmake -DCMAKE_CXX_STANDARD=17 -S. -Bbuild -DBUILD_DEPS:BOOL=ON && \
  cd build && make -j$(nproc) && make install && \
  echo "--- installed or-tools ---" && \
  apt-get clean

RUN git clone --recurse-submodules -b v1.50.0 \
  --depth 1 --shallow-submodules https://github.com/grpc/grpc

RUN cd grpc && mkdir -p cmake/build && cd ./cmake/build && \
  cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DABSL_PROPAGATE_CXX_STD=TRUE ../.. \
  -DgRPC_INSTALL=ON \                                   
  -DCMAKE_BUILD_TYPE=Release       \                                                
  -DgRPC_ABSL_PROVIDER=package     \                                                
  -DgRPC_CARES_PROVIDER=module   \                                                  
  -DgRPC_PROTOBUF_PROVIDER=package \
  -DgRPC_RE2_PROVIDER=package      \
  -DgRPC_SSL_PROVIDER=module      \ 
  -DgRPC_ZLIB_PROVIDER=package && \
  make -j$(nproc) && make install && \
  echo "--- installed grpc ---" && \
  apt-get clean                    

WORKDIR /Optimizer
COPY . .
RUN mkdir -p ./build && cd ./build && cmake .. && make -j$(nproc)
EXPOSE 50051
CMD ["./bin/server"]

