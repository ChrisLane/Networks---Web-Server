FROM alpine:3.4

RUN apk add --no-cache gcc make git libc-dev file-dev && \
    git clone https://github.com/ChrisLane/web-server /server && \
    cd /server && \
    make all && \
    apk del --no-cache gcc make git

WORKDIR /server

CMD /server/server 8088 public_html
