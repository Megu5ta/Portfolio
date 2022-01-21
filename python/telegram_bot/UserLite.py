class UserLite:
    def __init__(self, user_info):
        self.id_ = None
        self.tracked = list()
        self.convert = None
        self.unpack_user_info(user_info)

    def unpack_user_info(self, user_info):
        self.id_ = user_info[0]
        self.convert = user_info[2]

        if user_info[1]:
            self.tracked = user_info[1].split(',')

    def pack_user_info(self):
        self.tracked = set(self.tracked)
        tracked_currencies = ','.join(self.tracked)
        return (self.id_, tracked_currencies, self.convert)
