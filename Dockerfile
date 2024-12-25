# Use an official Ubuntu base image
FROM ubuntu:20.04

# Set environment variables to avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    curl \
    && apt-get clean

# Set the working directory
WORKDIR /app

# Copy the project files into the container
COPY . /app

# Create a build directory
RUN mkdir build

# Build the application
WORKDIR /app/build
RUN cmake .. && make

# Set the entry point to run the compiled program
CMD ["./OrderBookAggregator"]

