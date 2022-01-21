class User:
    def __init__(self, users_info):
        # users_info: tuple(id, convert, tracked)
        self.id_ = users_info[0]

        self.tracked_currencies = None    # IDs *strings*
        self.tracked_from_string_to_list(users_info[1])

        self.convert = "EUR"
        self.set_convert(users_info[2])

    def set_convert(self, convert):
        if convert in ["EUR", "USD"]:
            self.convert = convert

    def add_to_tracked(self, currency_id):
        if currency_id not in self.tracked_currencies:
            self.tracked_currencies.append(currency_id)

    def remove_from_tracked(self, currency_id):
        try:
            self.tracked_currencies.remove(currency_id)
        except ValueError:
            pass

    def to_database(self):
        tracked_currencies_csv_string = None
        if self.tracked_currencies:
            tracked_currencies_csv_string = ','.join(self.tracked_currencies)
        return (self.id_, tracked_currencies_csv_string, self.convert)

    def tracked_from_string_to_list(self, tracked):
        if tracked:
            self.tracked_currencies = tracked.split(',')
        else:
            self.tracked_currencies = list()

    def get_tracked_symbols(self):
        pass


if __name__ == "__main__":
    with open("./../hw.txt", "r", encoding="utf-8") as f:
        print(f.read())
