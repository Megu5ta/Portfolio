import sqlite3 as sq

db_name = "currencies_db.db"


def set_db_path(path):
    global db_name
    db_name = path


def get_currency_by_id(currency_id):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = '{}';".format("currencies"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM currencies WHERE currency_id={currency_id};")
            return cur.fetchall()

    return None


def get_currency_by_name(currency_name):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = '{}';".format("currencies"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM currencies WHERE currency_name='{currency_name}';")
            return cur.fetchall()

    return None


def get_currency_by_slug(currency_slug):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = '{}';".format("currencies"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM currencies WHERE currency_slug='{currency_slug}';")
            return cur.fetchall()

    return None


def get_currency_by_symbol(currency_symbol):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = '{}';".format("currencies"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM currencies WHERE currency_symbol='{currency_symbol}';")
            return cur.fetchall()

    return None
