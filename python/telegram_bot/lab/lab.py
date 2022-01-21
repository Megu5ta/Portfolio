from datetime import datetime
import time


# every_5_seconds = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55]
# now = datetime.now()
# seconds = now.second
#
# while seconds > every_5_seconds[0]:
#     popped = every_5_seconds.pop(0)
#     every_5_seconds.append(popped)
#
# hours = list(range(24))
# print(hours)
#
# while True:
#     now = datetime.now()
#     seconds = now.second
#     if seconds == every_5_seconds[0]:
#         popped = every_5_seconds.pop(0)
#         every_5_seconds.append(popped)
#         print("\n5 seconds")
#     time.sleep(0.5)
#     print('.', end='')

def shift_time(next_hour, max_hour):
  next_hour += 1
  if next_hour > max_hour:
    next_hour = 0
  return next_hour


next_hour = 0
max_hour = 59
current_hour = datetime.now().second
while current_hour >= next_hour:
    next_hour = shift_time(next_hour, max_hour)

msg = "Hello my sacrilegious supermegahacker3047\nCurrent hour: {}\nNext hour:{}".format(current_hour, next_hour)

while True:
    current_hour = datetime.now().second
    if current_hour == next_hour:
      next_hour = shift_time(next_hour, max_hour)
      print("Current: {}\nNext: {}".format(current_hour, next_hour))

    time.sleep(0.5)
