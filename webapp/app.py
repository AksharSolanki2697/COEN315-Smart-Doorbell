import requests
from flask import Flask
from flask import render_template

app = Flask(__name__)

@app.route('/button_pressed')
# Request the Camera when button pressed 

def button_pressed():
        
    URL = ""
           
    r = requests.get(url = URL)
    return r.content
    

@app.route('/image_captured')
# Get the the detected face and name
    
def image_captured():

    url = ''
    files = {'media': open('test.jpg', 'rb')}
    requests.post(url, files=files)


@app.route('/send_email')
#Get the ESP32 to send the email



@app.route('/')
def index():
    return render_template('index.html')


@app.route('/register')
def register():
    return render_template('register.html')


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
    
    
