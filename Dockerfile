FROM rikorose/gcc-cmake
COPY ./app /usr/app
WORKDIR /usr/app
RUN cmake .
RUN make