# Image of server application, handling image processing and data store

ARG BASE_IMAGE=debian:latest
FROM ${BASE_IMAGE}

    # Basic config
    LABEL maintainer="VinTRiZz <andzig501@gmail.com>"
    LABEL description="Сервер менеджмента для детекторов"
    ENV DEBIAN_FRONTENT=noninteractive
    ENV TZ=Etc/UTC
    ENV LANG=C.UTF-8
    ENV PROJECT_ROOTDIR=/server

    # Deps for building
    RUN apt update
    RUN apt install -y cmake make g++ build-essential ninja-build libboost-all-dev
    RUN apt install -y uuid-dev libpq5 libmariadb3
    RUN apt install -y libsqlite3-dev
    RUN apt install -y libssl-dev libsodium-dev
    RUN apt install -y nlohmann-json3-dev
    RUN apt install -y libjsoncpp-dev

    # Clean unnsessesary cache
    RUN apt clean && rm -rf /var/lib/apt/lists/*

    # Copy project
    ENV BUILD_TMPDIR=${PROJECT_ROOTDIR}/tmp
    WORKDIR ${BUILD_TMPDIR}
    COPY Apps/ ./Apps/
    COPY Libraries/ ./Libraries/
    COPY ComponentsSystem/ ./ComponentsSystem/
    COPY 3rdparty/ ./3rdparty/
    COPY CMakeLists.txt ./

    # Server user setup
    ENV SERVER_USER=server_user
    RUN useradd -ms /bin/bash ${SERVER_USER}
    RUN chown ${SERVER_USER}:${SERVER_USER} -R ${PROJECT_ROOTDIR}
    USER ${SERVER_USER}

    # Build project
    WORKDIR ${BUILD_TMPDIR}
    RUN mkdir build
    RUN cmake -S . -B ./build \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_CTL=OFF \
        -DBUILD_DOC=OFF \
        -DBUILD_MYSQL=OFF \
        -DBUILD_REDIS=OFF \
        -DBUILD_POSTGRESQL=OFF \
        -DBUILD_SQLITE=OFF \
        -DENABLE_PCC11=OFF \
        -DROD_BUILD_SERVER=ON \
        -DROD_BUILD_DETECTOR=OFF
    WORKDIR ${BUILD_TMPDIR}/build
    RUN cmake --build . --target install -- -j$(nproc)

    # Place to execution dir
    RUN mv ${BUILD_TMPDIR}/BIN/* ${PROJECT_ROOTDIR}
    RUN rm -rf ${BUILD_TMPDIR}

    # HTTP API for management and detectors
    ENV HTTP_API_PORT=9001
    EXPOSE ${HTTP_API_PORT}

    # WSS events to notify management panel
    ENV WSS_EVENT_PORT=9002
    EXPOSE ${WSS_EVENT_PORT}

    # UDP broadcast streaming port for images detector see
    ENV UDP_STREAM_PORT=9003
    EXPOSE ${UDP_STREAM_PORT}

    # Startup
    WORKDIR ${PROJECT_ROOTDIR}
    ENV LD_LIBRARY_PATH=${PROJECT_ROOTDIR}/lib
    CMD [ "sh", "-c", "exec ./ROD-Server.bin -e ${WSS_EVENT_PORT} -a ${HTTP_API_PORT} -s ${UDP_STREAM_PORT}" ]
