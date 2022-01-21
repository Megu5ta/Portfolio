import DBs.UsersDB_Manager as UserMan
import DBs.CurrenciesDB_Manager as CurrMan
from CryptoInfoBot import CryptoInfoBot
from UserLite import UserLite


info_bot = CryptoInfoBot()
UserMan.set_db_path("DBs/users_db.db")
CurrMan.set_db_path("DBs/currencies_db.db")


def get_currency_by(attribute, parameter):
    '''
    :param attribute: find by one of this attributes: "id", "name", "slug" or "symbol"
    :param parameter: value of attribute
    :return: if attribute is wrong - return None; returns list of found currencies in format ("id", "name", "slug", "symbol") or empty list if not found
    '''
    if attribute == "id":
        return CurrMan.get_currency_by_id(parameter)
    elif attribute == "name":
        return CurrMan.get_currency_by_name(parameter)
    elif attribute == "slug":
        return CurrMan.get_currency_by_slug(parameter)
    elif attribute == "symbol":
        return CurrMan.get_currency_by_symbol(parameter)

    return None


def add_user(user_id):
    UserMan.add_user(user_id)
    pass


def remove_user(user_id):
    UserMan.remove_user(user_id)
    pass


def get_user_by_id(user_id):
    return UserMan.get_user_by_id(user_id)


def add_to_users_tracked(user_id, id_to_track):
    id_to_track = str(id_to_track)
    user = UserMan.get_user_by_id(user_id)
    if not user:
        UserMan.add_user(user_id)
        user = UserMan.get_user_by_id(user_id)

    user = UserLite(user)
    user.tracked.append(id_to_track)
    id_, tracked_currencies, convert = user.pack_user_info()
    UserMan.update_users_tracked(id_, tracked_currencies)

    info_bot.track_currency(id_to_track)


def remove_from_users_tracked(user_id, id_to_untrack):
    user = UserMan.get_user_by_id(user_id)
    if not user:
        return False

    user = UserLite(user)
    try:
        user.tracked.remove(id_to_untrack)
        id_, tracked_currencies, convert = user.pack_user_info()
        UserMan.update_users_tracked(id_, tracked_currencies)
        return True
    except ValueError:
        return False


def update_users_tracked(user_id, tracked_currencies):
    UserMan.update_users_tracked(user_id, tracked_currencies)
    pass


def update_users_convert(user_id, convert):
    UserMan.update_users_convert(user_id, convert)
    pass


def all_users_gen():
    # Maybe this is very stupid...
    for user in UserMan.all_users_gen():
        yield user


def update_info():
    info_bot.update_info()


def messages_gen():
    users_to_remove = list()
    tracked_currencies = set()
    for user in UserMan.all_users_gen():
        current_user = UserLite(user)
        if current_user.tracked:
            tracked_currencies.update(current_user.tracked)
        else:
            users_to_remove.append(current_user.id_)
            continue

        msg = "Price in {}\n".format(current_user.convert)
        counter = 1
        for currency_id in current_user.tracked:
            info = info_bot.get_info(currency_id)
            if info is not None:
                msg += "[{:<8}price: {}\n".format(info["symbol"]+']', info["price"])
                counter += 1
                if counter % 2:
                    msg += '----\n'

        yield (current_user.id_, msg)

    info_bot.clean_up_tracked(list(tracked_currencies))
    for user_id in users_to_remove:
        remove_user(user_id)


def clean_up():
    users_to_remove = list()
    tracked_currencies = set()
    for user in UserMan.all_users_gen():
        current_user = UserLite(user)
        if current_user.tracked:
            tracked_currencies.update(current_user.tracked)
        else:
            users_to_remove.append(current_user.id_)

    info_bot.clean_up_tracked(list(tracked_currencies))
    for user_id in users_to_remove:
        remove_user(user_id)
