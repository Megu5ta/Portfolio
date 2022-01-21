from dotenv import load_dotenv
from telebot import types
import os
import telebot
import Director
from datetime import datetime
import time


load_dotenv()
# API_KEY = os.environ["API_KEY"]
TELEGRAM_API_KEY = os.getenv("TELEGRAM_API_KEY2")
bot = telebot.TeleBot(TELEGRAM_API_KEY)


@bot.message_handler(commands=["help"])
def print_help(message):
  text = {'EN': list(), 'RU': list()}
  EN_text = list()
  RU_text = list()

  EN_text.append('/tracked - shows currencies tracked by you')
  RU_text.append('/tracked - показывает отслеживаемые вами валюты')
  EN_text.append('track SYMBOL - adds currency with symbol SYMBOL to your tracklist\n\tExmaple: track BTC')
  RU_text.append('track SYMBOL - добавляет валюту с символом SYMBOL в отслеживаемые вами валюты\n\tПример: track BTC')
  EN_text.append('untrack SYMBOL - remove currency with symbol SYMBOL from your tracklist\n\tExample: untrack BTC')
  RU_text.append('untrack SYMBOL - убирает валюту с символом SYMBOL с вашего списка отслеживаемых валют\n\tПример? untrack BTC')
  EN_text.append('/help - shows help info')
  RU_text.append('/help - показывает воспомагающую информацию')


  text['EN'] = EN_text
  text['RU'] = RU_text


@bot.message_handler(commands=["tracked"])
def tracked(message):
  user = Director.get_user_by_id(message.chat.id)
  if user:
    tracked = user[1].split(',')
    if not tracked[0]:
      bot.send_message(message.chat.id, "You don't track any currency")
      return
    msg = "Your tracked currencies:\n"
    for currency_id in tracked:
      currency = Director.get_currency_by("id", currency_id)[0]
      # Need test to check when currency = None(or empty list)
      # print(currency)
      msg += "[{}] {}\n".format(currency[3], currency[1])   # [symbol] Name
    bot.send_message(message.chat.id, msg)
  else:
    bot.send_message(message.chat.id, "Can't find the currencies you are tracking")


def track_request(message):
  request = message.text.split()
  if len(request) < 2 or request[0].lower() != "track":
    return False
  return True


@bot.message_handler(func=track_request)
def track_currency(message):
  currency_symbol = message.text.split()[1].upper()
  result = Director.get_currency_by("symbol", currency_symbol)
  if not result:
    msg = "Currency with symbol {} not found".format(currency_symbol)
    bot.send_message(message.chat.id, msg)
    print(msg)
    return

  length = len(result)

  if length > 1:
    markup = types.InlineKeyboardMarkup(row_width=length)
    items = list()
    for currency in result:
      text = "[{}] {}".format(currency[3], currency[1])
      callback_data = "track {}".format(currency[0])
      item = types.InlineKeyboardButton(text, callback_data=callback_data)
      items.append(item)
    for item in items:
      markup.add(item)
    bot.send_message(message.chat.id, "Which one do you want to track?", reply_markup=markup)
  else:
    result = result[0]
    Director.add_to_users_tracked(message.chat.id, result[0])
    msg = "[{}] {} successfully added to you tracklist".format(result[3], result[1])
    bot.send_message(message.chat.id, msg)


def untrack_request(message):
  request = message.text.split()
  if len(request) < 2 or request[0].lower() != "untrack":
    return False
  return True


@bot.message_handler(func=untrack_request)
def untrack_currency(message):
  # OMG wtf is going on here? Maybe someday i'll make it looks better, but now it should at least not crash
  currency_symbol = message.text.split()[1].upper()
  user = Director.get_user_by_id(message.chat.id)
  msg = ""
  if not user:
    msg = "{} is not tracked by you".format(currency_symbol)
  else:
    result = Director.get_currency_by("symbol", currency_symbol)
    if not result:
      msg = "Currency with symbol {} not found".format(currency_symbol)
      bot.send_message(message.chat.id, msg)
      print(msg)
      return

    length = len(result)
    tracked_currencies = user[1].split(',')

    if length > 1:
      is_in_tracked = list()
      for currency in result:
        currency_id = str(currency[0])
        if currency_id in tracked_currencies:
          is_in_tracked.append(currency)

      if not is_in_tracked:
        msg = "You don't track {} now".format(currency_symbol)
      elif len(is_in_tracked) > 1:
        markup = types.InlineKeyboardMarkup(row_width=len(is_in_tracked))
        items = list()
        for currency in is_in_tracked:
          text = "[{}] {}".format(currency[3], currency[1])
          callback_data = "untrack {}".format(currency[0])
          item = types.InlineKeyboardButton(text, callback_data=callback_data)
          items.append(item)
        for item in items:
          markup.add(item)
        bot.send_message(message.chat.id, "Which one do you want to untrack?", reply_markup=markup)
        return
      else:
        print(is_in_tracked)
        id_to_untrack = str(is_in_tracked[0][0])
        Director.remove_from_users_tracked(message.chat.id, id_to_untrack)
        msg = "You don't track {} now".format(currency_symbol)
    else:
      id_to_untrack = str(result[0][0])
      Director.remove_from_users_tracked(message.chat.id, id_to_untrack)
      msg = "You don't track {} now".format(currency_symbol)

  bot.send_message(message.chat.id, msg)
  # It's nasty. I know. I just hope that everything will work perfectly and no one will look here.


@bot.callback_query_handler(func=lambda call: True)
def callback_inline(call):
  if call.message:
    starts_with = call.data.split()[0]
    parameter = call.data.split()[1]
    if starts_with == "track":
      Director.add_to_users_tracked(call.message.chat.id, parameter)
      bot.send_message(call.message.chat.id, "You track it now")
    elif starts_with == "untrack":
      Director.remove_from_users_tracked(call.message.chat.id, parameter)
      bot.send_message(call.message.chat.id, "You don't track it now")

    bot.delete_message(chat_id=call.message.chat.id, message_id=call.message.message_id)
    # bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=call.message.text, reply_markup=None)


@bot.message_handler(commands=["info"])
def info(message):
  Director.update_info()
  for user_id, msg in Director.messages_gen():
    bot.send_message(user_id, msg)


def shift_time(next_hour, max_hour):
  next_hour += 1
  if next_hour > max_hour:
    next_hour = 0
  return next_hour


def activation_request(message):
  return message.text == os.getenv("SECRET__ACTIVATION_WORD")


@bot.message_handler(func=activation_request)
def timer(message):
  next_hour = 0
  max_hour = 23
  current_hour = datetime.now().hour
  while current_hour >= next_hour:
    next_hour = shift_time(next_hour, max_hour)

  msg = "Hello my sacrilegious supermegahacker3047\nCurrent hour: {}\nNext hour:{}".format(current_hour, next_hour)
  bot.send_message(message.chat.id, msg)

  while True:
    current_hour = datetime.now().hour
    if current_hour == next_hour:
      next_hour = shift_time(next_hour, max_hour)

      Director.update_info()
      for user_id, msg in Director.messages_gen():
        bot.send_message(user_id, msg)
        print(user_id)
        print(msg)
        print('-'*10)

    time.sleep(600)


@bot.channel_post_handler(commands=["hello"])
def hello(message):
  bot.send_message(message.chat.id, str(message.chat.id))


bot.polling()
