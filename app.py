import requests
import json

class OrderLevel:
    def __init__(self, price, size, exchange):
        self.price = price
        self.size = size
        self.exchange = exchange

class SendOrder:
    def __init__(self, exchange, order_type, price, limit):
        self.exchange = exchange
        self.type = order_type
        self.price = price
        self.limit = limit

class LimitOrder:
    def __init__(self, exchange, order_type, limit_price, limit_size):
        self.exchange = exchange
        self.type = order_type
        self.limit_price = limit_price
        self.limit_size = limit_size

def fetch_data(url):
    response = requests.get(url)
    if response.status_code == 200:
        return response.text
    else:
        print(f"Failed to fetch data from {url}")
        return ""

def parse_coinbase(data):
    bids, asks = [], []
    try:
        root = json.loads(data)
        for bid in root["bids"]:
            bids.append(OrderLevel(float(bid[0]), float(bid[1]), "Coinbase"))
        for ask in root["asks"]:
            asks.append(OrderLevel(float(ask[0]), float(ask[1]), "Coinbase"))
    except Exception as e:
        print(f"Error parsing Coinbase data: {e}")
    return bids, asks

def parse_gemini(data):
    bids, asks = [], []
    try:
        root = json.loads(data)
        for bid in root["bids"]:
            bids.append(OrderLevel(float(bid["price"]), float(bid["amount"]), "Gemini"))
        for ask in root["asks"]:
            asks.append(OrderLevel(float(ask["price"]), float(ask["amount"]), "Gemini"))
    except Exception as e:
        print(f"Error parsing Gemini data: {e}")
    return bids, asks

def parse_kraken(data):
    bids, asks = [], []
    try:
        root = json.loads(data)
        result = root.get("result", {})
        if result:
            key = list(result.keys())[0]
            order_book = result[key]
            for bid in order_book["bids"]:
                bids.append(OrderLevel(float(bid[0]), float(bid[1]), "Kraken"))
            for ask in order_book["asks"]:
                asks.append(OrderLevel(float(ask[0]), float(ask[1]), "Kraken"))
    except Exception as e:
        print(f"Error parsing Kraken data: {e}")
    return bids, asks

def merge_order_books(all_bids, all_asks, book):
    all_bids.extend(book[0])
    all_asks.extend(book[1])
    all_bids.sort(key=lambda x: x.price, reverse=True)
    all_asks.sort(key=lambda x: x.price)

def calculate_price(levels, quantity):
    total_cost = 0.0
    remaining = quantity

    for level in levels:
        if remaining <= 0:
            break
        trade_size = min(level.size, remaining)
        total_cost += trade_size * level.price
        remaining -= trade_size

    if remaining > 0:
        print("Not enough liquidity to fulfill the order.")
        return -1.0

    return total_cost

def sweep_order_book(orders, order_type, levels, quantity):
    total_cost = 0.0
    remaining = quantity

    for level in levels:
        if remaining <= 0:
            break
        trade_size = min(level.size, remaining)
        total_cost += trade_size * level.price
        remaining -= trade_size
        orders.append(SendOrder(level.exchange, order_type, level.price, trade_size))

    if remaining > 0:
        print("Not enough liquidity to fulfill the order.")

def calculate_limit_order(exchange, orders, order_type):
    limit_price = 0
    limit_size = 0

    for order in orders:
        if order.exchange == exchange:
            if limit_size == 0:
                limit_price = order.price
            limit_price = max(limit_price, order.price) if order_type == "Buy" else min(limit_price, order.price)
            limit_size += order.limit

    return LimitOrder(exchange, order_type, limit_price, limit_size)

if __name__ == "__main__":
    coinbase_data = fetch_data("https://api.exchange.coinbase.com/products/BTC-USD/book?level=2")
    gemini_data = fetch_data("https://api.gemini.com/v1/book/BTCUSD")
    kraken_data = fetch_data("https://api.kraken.com/0/public/Depth?pair=BTCUSD")

    coinbase = parse_coinbase(coinbase_data)
    gemini = parse_gemini(gemini_data)
    kraken = parse_kraken(kraken_data)

    all_bids, all_asks = [], []
    merge_order_books(all_bids, all_asks, coinbase)
    merge_order_books(all_bids, all_asks, gemini)
    merge_order_books(all_bids, all_asks, kraken)

    quantity = float(input("Enter quantity of BTC to buy/sell: "))

    buy_total = calculate_price(all_asks, quantity)
    sell_total = calculate_price(all_bids, quantity)

    if buy_total >= 0:
        print(f"Cost to buy {quantity} BTC: ${buy_total:.2f}")
    if sell_total >= 0:
        print(f"Proceeds from selling {quantity} BTC: ${sell_total:.2f}")

    buy_orders, sell_orders = [], []
    sweep_order_book(buy_orders, "Buy", all_asks, quantity)
    sweep_order_book(sell_orders, "Sell", all_bids, quantity)

    exchanges = ["Coinbase", "Gemini", "Kraken"]

    for exchange in exchanges:
        buy_limit_order = calculate_limit_order(exchange, buy_orders, "Buy")
        sell_limit_order = calculate_limit_order(exchange, sell_orders, "Sell")

        print(f"{buy_limit_order.exchange} Buy Limit Order: [Price: ${buy_limit_order.limit_price}, Size: {buy_limit_order.limit_size} BTC]")
        print(f"{sell_limit_order.exchange} Sell Limit Order: [Price: ${sell_limit_order.limit_price}, Size: {sell_limit_order.limit_size} BTC]")


