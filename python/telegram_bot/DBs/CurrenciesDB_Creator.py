import sqlite3 as sq
import json
import csv


db_name = "currencies_db.db"


def load_json_data(filepath):
    with open(filepath, "r", encoding="utf-8") as fr:
        data = json.load(fr)
        return data


def json_to_csv(json_data, output_file):
    with open(output_file, "w", encoding="utf-8") as fw:
        fw.write("id;name;slug;symbol\n")
        for key, value in json_data.items():
            line = ';'.join([key, value["name"], value["slug"], value["symbol"]])
            fw.write(line + '\n')


def create_db(csv_filepath):
    with open(csv_filepath, "r", encoding="utf-8") as fr:
        rows = csv.reader(fr, delimiter=';')
        next(rows, None)
        with sq.connect(db_name) as con:
            cur = con.cursor()
            cur.execute("""CREATE TABLE currencies (
            currency_id INTEGER PRIMARY KEY,
            currency_name TEXT NOT NULL,
            currency_slug TEXT NOT NULL,
            currency_symbol TEXT NOT NULL
            );""")

            cur.executemany("INSERT INTO currencies VALUES(?, ?, ?, ?);", rows)
            con.commit()


# def get_all_currencies():
#     with sq.connect(db_name) as con:
#         cur = con.cursor()
#         cur.execute("SELECT * FROM currencies;")
#         print(cur.fetchall())


if __name__ == "__main__":
    pass
    # data = load_json_data("cryptoMainInfo.txt")
    # json_to_csv(data, "output.csv")
    # create_db("output.csv")
    # get_all_currencies()
