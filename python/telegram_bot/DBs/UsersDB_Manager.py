import sqlite3 as sq
import csv

db_name = "users_db.db"


def set_db_path(path):
    global db_name
    db_name = path


def create_table_users():
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("""CREATE TABLE IF NOT EXISTS users (
          user_id INTEGER PRIMARY KEY,
          tracked_currencies TEXT,
          convert TEXT DEFAULT 'EUR'
          );""")
        con.commit()


def add_user(user_id):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute(f"INSERT OR IGNORE INTO users(user_id) VALUES('{user_id}');")
            con.commit()


def get_user_by_id(user_id):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM users WHERE user_id={user_id};")
            return cur.fetchone()


def remove_user(user_id):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute(f"DELETE FROM users WHERE user_id = {user_id};")
            con.commit()


def update_users_tracked(user_id, tracked_currencies):
    '''
    If table users exists update users tracked_currencies. If user not exists then add user to table first
    :param user_id: users id (or chat.id)
    :param tracked_currencies: csv string with tracked_currencies ids. For example: "1,2,5,7,120"
    :return:
    '''
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute(f"SELECT * FROM users WHERE user_id={user_id};")
            if not cur.fetchone():
                add_user(user_id)
            cur.execute(f"UPDATE users SET tracked_currencies = '{tracked_currencies}' WHERE user_id = {user_id};")
            con.commit()


def update_users_convert(user_id, convert):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute(f"UPDATE users SET convert = '{convert}' WHERE user_id = {user_id};")
            con.commit()


def update_user(user):
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            user_id, tracked_currencies, convert = user
            # CHECK IT AGAIN
            cur.execute(f"UPDATE users SET tracked_currencies = '{tracked_currencies}', convert = '{convert}' WHERE user_id = {user_id};")
            con.commit()


def all_users_gen():
    with sq.connect(db_name) as con:
        cur = con.cursor()
        cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='{}';".format("users"))
        if cur.fetchone():
            cur.execute("SELECT * FROM users;")
            while(user := cur.fetchone()) is not None:
                yield user


def clean_up(list_of_users_id_to_remove):
    for id_ in list_of_users_id_to_remove:
        remove_user(id_)


# lst = list()
# for user in all_users_gen():
#     lst += user[1].split(',')
#
# lst = list(set(lst))
# with open("tracked", "w", encoding="utf-8") as fp:
#     fp.write(','.join(lst))

for user in all_users_gen():
    print(user)
# create_table_users()
# add_user("100")
# add_user("200")
# add_user("300")
#
# update_users_tracked("100", "1,2,52,74")
# update_users_tracked("200", "1,2,1765")
# update_users_tracked("300", "3718")
