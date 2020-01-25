from flask import Flask
from flask_restful import Resource, reqparse, Api
import json
from flask_cors import CORS, cross_origin

app = Flask(__name__)
cors = CORS(app)
api = Api(app)
app.config['CORS_HEADERS'] = 'Content-Type'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///base.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['PROPAGATE_EXCEPTIONS'] = True

from base import Data, db
db.init_app(app)
app.app_context().push()
db.create_all()

class MyApp(Resource):
    parser = reqparse.RequestParser()
    parser.add_argument('data', type=str)

    def get(self, timestamp):
        item = Data.find_by_timestamp(timestamp)
        if item:
            return item.json()
        return {'Message': 'Movie is not found'}

    def post(self):
        args = MyApp.parser.parse_args()
        print("#"*80)
        print(args['data'])
        json_data = json.loads(args['data'])
        item = Data(json_data['timestamp'],
                    json_data['longitude'],
                    json_data['latitude'],
                    json_data['temperature'],
                    json_data['humidity'],
                    json_data['sound'],
                    json_data['co'],
                    json_data['co2'],
                    json_data['gpl'],
                    json_data['waste'])
        item.save_to()
        return item.json()

class Find_all(Resource):
    def get(self):
        return {'data' : list(map(lambda x: x.json(), Data.query.all()))}

class Root(Resource):
    def get(self):
        return {
            'status': 'OK',
        }

api.add_resource(Root, '/')
api.add_resource(Find_all, '/all')
api.add_resource(MyApp, '/data')

if __name__=='__main__':

    app.run(debug=True)