#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <curl/curl.h>
#include <json/json.h>

// Helper to store order book levels
struct OrderLevel {
    double price;
    double size;
};

// Helper function for CURL write callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to fetch data from a URL
std::string fetchData(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

// Parse Coinbase Pro order book
std::pair<std::vector<OrderLevel>, std::vector<OrderLevel>> parseCoinbase(const std::string& data) {
    Json::Reader reader;
    Json::Value root;
    std::vector<OrderLevel> bids, asks;

    if (reader.parse(data, root)) {
        for (const auto& bid : root["bids"]) {
            bids.push_back({std::stod(bid[0].asString()), std::stod(bid[1].asString())});
        }
        for (const auto& ask : root["asks"]) {
            asks.push_back({std::stod(ask[0].asString()), std::stod(ask[1].asString())});
        }
    }

    return {bids, asks};
}

// Parse Gemini order book
std::pair<std::vector<OrderLevel>, std::vector<OrderLevel>> parseGemini(const std::string& data) {
    Json::Reader reader;
    Json::Value root;
    std::vector<OrderLevel> bids, asks;

    if (reader.parse(data, root)) {
        for (const auto& bid : root["bids"]) {
            bids.push_back({std::stod(bid["price"].asString()), std::stod(bid["amount"].asString())});
        }
        for (const auto& ask : root["asks"]) {
            asks.push_back({std::stod(ask["price"].asString()), std::stod(ask["amount"].asString())});
        }
    }

    return {bids, asks};
}

std::pair<std::vector<OrderLevel>, std::vector<OrderLevel>> parseKraken(const std::string& data) {
    Json::Reader reader;
    Json::Value root;
    std::vector<OrderLevel> bids, asks;

    // Parse JSON data
    if (reader.parse(data, root)) {
        const auto& result = root["result"];

        // Ensure "result" is a valid JSON object
        if (!result.isObject() || result.empty()) {
            std::cerr << "Invalid Kraken result format" << std::endl;
            return {bids, asks};
        }

        // Get the first key (e.g., "XXBTZUSD") from the result
        const std::string key = result.getMemberNames().front();
        const auto& orderBook = result[key];

        // Parse bids
        for (const auto& bid : orderBook["bids"]) {
            if (bid.isArray() && bid.size() >= 2) {
                double price = std::stod(bid[0].asString());
                double size = std::stod(bid[1].asString());
                bids.push_back({price, size});
            }
        }

        // Parse asks
        for (const auto& ask : orderBook["asks"]) {
            if (ask.isArray() && ask.size() >= 2) {
                double price = std::stod(ask[0].asString());
                double size = std::stod(ask[1].asString());
                asks.push_back({price, size});
            }
        }
    } else {
        std::cerr << "Failed to parse Kraken JSON data" << std::endl;
    }

    return {bids, asks};
}



// Merge and sort order books
void mergeOrderBooks(std::vector<OrderLevel>& allBids, std::vector<OrderLevel>& allAsks,
                     const std::pair<std::vector<OrderLevel>, std::vector<OrderLevel>>& book) {
    allBids.insert(allBids.end(), book.first.begin(), book.first.end());
    allAsks.insert(allAsks.end(), book.second.begin(), book.second.end());
    std::sort(allBids.rbegin(), allBids.rend(), [](const OrderLevel& a, const OrderLevel& b) { return a.price < b.price; });
    std::sort(allAsks.begin(), allAsks.end(), [](const OrderLevel& a, const OrderLevel& b) { return a.price < b.price; });
}

// Calculate the price to buy/sell specified BTC
double calculatePrice(const std::vector<OrderLevel>& levels, double quantity) {
    double totalCost = 0.0;
    double remaining = quantity;

    for (const auto& level : levels) {
        if (remaining <= 0) break;
        double tradeSize = std::min(level.size, remaining);
        totalCost += tradeSize * level.price;
        remaining -= tradeSize;
    }

    if (remaining > 0) {
        std::cerr << "Not enough liquidity to fulfill the order.\n";
        return -1.0;
    }

    return totalCost;
}

int main() {
    std::string coinbaseData = fetchData("https://api.exchange.coinbase.com/products/BTC-USD/book?level=2");
    std::string geminiData = fetchData("https://api.gemini.com/v1/book/BTCUSD");
    std::string krakenData = fetchData("https://api.kraken.com/0/public/Depth?pair=BTCUSD");

    auto coinbase = parseCoinbase(coinbaseData);
    auto gemini = parseGemini(geminiData);
    auto kraken = parseKraken(krakenData);

    std::vector<OrderLevel> allBids, allAsks;
    mergeOrderBooks(allBids, allAsks, coinbase);
    mergeOrderBooks(allBids, allAsks, gemini);
    mergeOrderBooks(allBids, allAsks, kraken);

    double quantity;
    std::cout << "Enter quantity of BTC to buy/sell: ";
    std::cin >> quantity;

    double buyPrice = calculatePrice(allAsks, quantity);
    double sellPrice = calculatePrice(allBids, quantity);

    if (buyPrice >= 0)
        std::cout << "Price to buy " << quantity << " BTC: $" << buyPrice << "\n";
    if (sellPrice >= 0)
        std::cout << "Price to sell " << quantity << " BTC: $" << sellPrice << "\n";

    return 0;
}
