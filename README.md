# Cryptocurrency Order Book Aggregator

This C++ project fetches and processes cryptocurrency order books from **Coinbase Pro**, **Gemini**, and **Kraken** exchanges. The program aggregates the order books, calculates the cost to buy or sell a specified quantity of Bitcoin, and outputs the results in the terminal.

## Features

### Fetch Order Books
- Retrieves real-time order book data from Coinbase Pro, Gemini, and Kraken.

### Normalize Data
- Standardizes order book data into a unified structure for easy processing.

### Merge and Sort
- Combines bids and asks from all exchanges while maintaining proper sorting:
    - **Bids**: Highest price first.
    - **Asks**: Lowest price first.

### Liquidity Taking
- Simulates buying or selling Bitcoin by sweeping through the combined order book to calculate the total cost or proceeds for a specified quantity.

### Console Interaction
- Allows the user to adjust the quantity of Bitcoin to buy or sell via terminal input.

---

## Prerequisites

### Libraries
The program uses the following libraries:
- **libcurl**: For HTTP requests.
- **JsonCpp**: For parsing JSON responses.

### Install Dependencies

#### Linux (Ubuntu/Debian)
```
sudo apt-get install libcurl4-openssl-dev libjsoncpp-dev
```
#### MacOS
```
brew install curl jsoncpp
```
#### Windows
```
vcpkg install curl jsoncpp
```

## Installation and Setup
### 1. Clone the Repository
Run the following commands to clone the repository and navigate to the project directory:
```
git clone https://github.com/eluzgin/crypto-orderbook-aggregator.git
cd crypto-orderbook-aggregator
```

### 2. Build the Project
Use the following commands to build the project:
```
mkdir build
cd build
cmake ..
make
```

### 3. Run the Program
Execute the compiled program:
```
./OrderBook
```

## Docker Support
### Build and Run Using Docker
### 1. Build the Docker Image:
```
docker build -t orderbook-aggregator .
```
### 2. Run the Docker Container:
```
docker run -it orderbook-aggregator
```
### 3. Interactive Development (Optional): 
During development, mount your source code into the container for quick iterations:
```
docker run -it -v $(pwd):/app orderbook-aggregator
```
### 4. Start an interactive shell in the container:
```
docker run -it --entrypoint /bin/bash orderbook-aggregator
```
### 5. Run program in Docker shell:
```
./OrderBook
```

### Example program output:
```
Enter quantity of BTC to buy/sell: 10
Cost to buy 10 BTC: $957780.50 at a price: $95778.05
Proceeds from selling 10 BTC: $958023.73 at a price: $95802.37
```


## Resources:
1. [Simulating a financial exchange in Scala](https://falconair.github.io/2015/01/05/financial-exchange.html)

### Author:
Eugene Luzgin, Dec 25, 2024  
````
         |
        -+-
         A
        /=\               /\  /\    ___  _ __  _ __ __    __
      i/ O \i            /  \/  \  / _ \| '__|| '__|\ \  / /
      /=====\           / /\  /\ \|  __/| |   | |    \ \/ /
      /  i  \           \ \ \/ / / \___/|_|   |_|     \  /
    i/ O * O \i                                       / /
    /=========\        __  __                        /_/    _
    /  *   *  \        \ \/ /        /\  /\    __ _  ____  | |
  i/ O   i   O \i       \  /   __   /  \/  \  / _` |/ ___\ |_|
  /=============\       /  \  |__| / /\  /\ \| (_| |\___ \  _
  /  O   i   O  \      /_/\_\      \ \ \/ / / \__,_|\____/ |_|
i/ *   O   O   * \i
/=================\
       |___|
````
