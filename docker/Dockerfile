### Build/test container ###
# Define builder stage
FROM git-gud:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure
### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/config/deploy_config .
COPY --from=builder /usr/src/project/folder1 ./folder1
COPY --from=builder /usr/src/project/folder2 ./folder2
COPY --from=builder /usr/src/project/folder3 ./folder3

EXPOSE 8080 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["deploy_config"]
