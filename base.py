from flask_sqlalchemy import SQLAlchemy
db = SQLAlchemy()

class Data(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.BigInteger, unique=False, nullable=False)
    longitude = db.Column(db.Float, unique=False, nullable=False)
    latitude = db.Column(db.Float, unique=False, nullable=False)
    temperature = db.Column(db.Float, unique=False, nullable=False)
    humidity = db.Column(db.Float, unique=False, nullable=False)
    sound = db.Column(db.Integer, unique=False, nullable=False)
    co = db.Column(db.Float, unique=False, nullable=False)
    co2 = db.Column(db.Float, unique=False, nullable=False)
    gpl = db.Column(db.Float, unique=False, nullable=False)
    waste = db.Column(db.Boolean, unique=False, nullable=False)

    def __init__(self, timestamp, longitude, latitude, temperature, humidity, sound, co, co2, gpl, waste):
        self.timestamp = timestamp
        self.longitude = longitude
        self.latitude = latitude
        self.temperature = temperature
        self.humidity = humidity
        self.sound = sound
        self.co = co
        self.co2 = co2
        self.gpl = gpl
        self.waste = waste

    def json(self):
        return {'timestamp': self.timestamp,
                'longitude': self.longitude,
                'latitude': self.latitude,
                'temperature': self.temperature,
                'humidity': self.humidity,
                'sound': self.sound,
                'co': self.co,
                'co2': self.co2,
                'gpl': self.gpl,
                'waste': self.waste}

    def json_temperature(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.temperature}

    def json_humidity(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.humidity}

    def json_sound(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.sound}

    def json_co(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.co}

    def json_co2(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.co2}

    def json_gpl(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.gpl}

    def json_waste(self):
        return {'latitude': self.latitude,
                'longitude': self.longitude,
                'value': self.waste}






    @classmethod
    def find_by_timestamp(cls, timestamp):
        pass

    def save_to(self):
        db.session.add(self)
        db.session.commit()

    def delete_(self):
        db.session.delete(self)
        db.session.commit()
