from flask import render_template, make_response, Flask
from flask_restful import Resource, reqparse, Api
from webargs import fields, validate
from webargs.flaskparser import use_kwargs, parser
import json

app = Flask(__name__, static_url_path="/static")
api = Api(app)
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
        print("args : {}\ttype : {}".format(args, type(args)))
        print("args['data'] : {}\ttype : {}".format(args['data'], type(args['data'])))
        print("#"*80)
        json_data = json.loads(args['data'])
        print("json : {}\ttype : {}".format(json_data, type(json_data)))
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

class Find_specific(Resource):
    args = {
        'type': fields.Str(
            required=True,
            validate=validate.OneOf(['temperature', 'air', 'waste', 'son', 'humidity']),
        ),
    }

    @use_kwargs(args)
    def get(self, type):
        if(type == 'temperature'):
            return list(map(lambda x: x.json_temperature(), Data.query.all()))
        elif(type == 'humidity'):
            return list(map(lambda x: x.json_humidity(), Data.query.all()))
        elif(type == 'son'):
            return list(map(lambda x: x.json_sound(), Data.query.all()))
        elif(type == 'air'):
            co = list(map(lambda x: x.json_co(), Data.query.all()))
            co2 = list(map(lambda x: x.json_co2(), Data.query.all()))
            gpl = list(map(lambda x: x.json_gpl(), Data.query.all()))
            return [co, co2, gpl]
        elif(type == 'waste'):
            return list(map(lambda x: x.json_waste(), Data.query.all()))
        else:
            return list()


class Find_all(Resource):
    def get(self):
        return {'data' : list(map(lambda x: x.json(), Data.query.all()))}

class Root(Resource):
    def get(self):
        response = make_response(render_template('/index.html'))
        response.headers["Content-Type"]='text/html'
        # return render_template('/index.html')
        return response



api.add_resource(Root, '/')
api.add_resource(Find_all, '/all')
api.add_resource(Find_specific, '/get', endpoint='get')
api.add_resource(MyApp, '/data')

if __name__=='__main__':

    app.run(debug=True)
