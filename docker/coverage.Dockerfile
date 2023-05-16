### Build/test container ###
# Define builder stage
FROM git-gud:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

# Build and generate coverage report
# RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
# RUN make coverage


### Deploy container ###
# Define deploy stage
# FROM ubuntu:jammy as deploy

# COPY --from=builder /usr/src/project/build/bin/server .
# COPY --from=builder /usr/src/project/config/deploy_config .

# EXPOSE 8080 80

# # Use ENTRYPOINT to specify the binary name
# ENTRYPOINT ["./server"]

# # Use CMD to specify arguments to ENTRYPOINT
# CMD ["deploy_config"]
