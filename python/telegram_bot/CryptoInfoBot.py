from requests import Request, Session
from dotenv import load_dotenv
import json
import os

tracked_filepath = "tracked"


class CryptoInfoBot:
    def __init__(self):
        self.latest_quotes_url = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest"
        self.ids = None
        self.load_tracked_ids()
        # self.converts = ['EUR', 'USD']
        load_dotenv()
        self.quotes_parameters = {
            "id": "1",
            "convert": "EUR"
        }
        self.headers = {
            "Accepts": "application/json",
            "X-CMC_PRO_API_KEY": os.getenv("MARKETCAP_API_KEY")
        }
        self.session = Session()
        self.session.headers.update(self.headers)
        self.last_response = None
        self.last_extracted = dict()

    def load_tracked_ids(self):
        with open(tracked_filepath, "r", encoding="utf-8") as fr:
            self.ids = fr.read().split(',')

    def save_tracked_ids(self):
        with open(tracked_filepath, "w", encoding="utf-8") as fw:
            fw.write(','.join(self.ids))

    def get_latest_quotes(self):
        # for convert in self.converts  <---------------------------
        id_param = ','.join(self.ids)
        self.quotes_parameters["id"] = id_param
        response = self.session.get(self.latest_quotes_url, params=self.quotes_parameters)
        # with open("firstatTestMultipleCoins.txt", "r", encoding="utf-8") as fp:
        #     response = json.load(fp)

        self.last_response = json.loads(response.text)
        # self.last_response = response

    def extract_info(self, response=None):
        if response is None:
            response = self.last_response
        if response is not None:
            self.last_extracted = dict()
            for currency_id, value in response["data"].items():
                info = {"name": value["name"],
                        "slug": value["slug"],
                        "symbol": value["symbol"],
                        "price": value["quote"]["EUR"]["price"]}
                self.last_extracted.update({currency_id: info})

    def get_info(self, currency_id):
        try:
            return self.last_extracted[currency_id]
        except KeyError:
            print(f"Currency with id {currency_id} not found")
        return None

    def update_info(self):
        self.get_latest_quotes()
        self.extract_info()

    def track_currency(self, id_to_track):
        if id_to_track not in self.ids:
            self.ids.append(id_to_track)
            self.quotes_parameters["id"] = ','.join(self.ids)
            self.save_tracked_ids()

    def untrack_currency(self, id_to_untrack):
        try:
            if isinstance(id_to_untrack, list):
                for currency_id in id_to_untrack:
                    self.ids.remove(currency_id)
            else:
                self.ids.remove(id_to_untrack)
            self.quotes_parameters["id"] = ','.join(self.ids)
            self.save_tracked_ids()
        except ValueError:
            pass

    def clean_up_tracked(self, actually_used):
        currencies_to_untrack = list()
        for currency_id in self.ids:
            if currency_id not in actually_used:
                currencies_to_untrack.append(currency_id)

        self.untrack_currency(currencies_to_untrack)
'''
# url = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest"
url = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/map"
# parameters = {
#     "id": "1,2,52,74,1765,1839,1975,2010,3718,5426,5893,6138",
#     "convert": "EUR"
# }

parameters = {
    "limit": 5000,
    "sort": "id"
}

headers = {
    "Accepts": "application/json",
    "X-CMC_PRO_API_KEY": "4e0d9b1c-54b3-4a0b-97c9-56bf9931acd2"
}

session = Session()
session.headers.update(headers)

response = session.get(url, params=parameters)

# pprint.pprint(json.loads(response.text))

with open("response.txt", "w", encoding="utf-8") as f:
    f.write(response.text)
'''
