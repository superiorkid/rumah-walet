from flask import Flask, render_template
from datetime import datetime
from dateutil import tz
import requests
import configparser
import re

app = Flask(__name__)


@app.route('/')
def index():
	title = 'Dashboard'
	return render_template('index.html', title=title)

@app.route('/about')
def about():
	title = "About us"
	return render_template('about.html', title=title)


@app.get('/data')
@app.post('/data')
def data():
	title = "Data"
	results = get_results()
	pattern = r'[TZ]'

	server_time = [re.sub(pattern, ' ', label['created_at'])[0:-1] for label in results['feeds']]

	wita_time = list()

	for _time in server_time:
		from_zone = tz.gettz('UTC')
		to_zone = tz.gettz('Asia/Makassar')

		utc = datetime.strptime(_time, '%Y-%m-%d %H:%M:%S')
		utc = utc.replace(tzinfo=from_zone)
		central = utc.astimezone(to_zone)
		central = central.strftime('%Y-%m-%d %H:%M:%S')
		wita_time.append(central)
		

	data = [
		# [range(len(server_time))],
		wita_time, # label
		[float(temp['field1']) for temp in results['feeds']], # temperature
		[float(hum['field2']) for hum in results['feeds']], # humidity
		# [float(press['field3']) for press in results['feeds']], # tekanan
		[float(alt['field4']) for alt in results['feeds']] # altitude
	]


	return render_template('data.html', data=data, title=title)


def get_results():
	ch_id = get_ch_id()
	read_api = get_api_key()		
	ts = 'https://api.thingspeak.com/channels/{}/feeds.json?api_key={}&results=12'.format(ch_id, read_api)

	resp = requests.get(ts)
	return resp.json()


def get_api_key():
	config = configparser.ConfigParser()
	config.read('config.ini')
	return config['thingspeak']['read_api']


def get_ch_id():
	config = configparser.ConfigParser()
	config.read('config.ini')
	return config['thingspeak']['ch_id']


if __name__ == '__main__':
	app.run(debug=True)
