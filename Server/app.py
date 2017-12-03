# Aqui se listan los imports
from flask import Flask
from flask import render_template
from flask import request

app = Flask(__name__)

frec = 1
servo1 = 90
servo2 = 90
servo3 = 90
servoSelect = 1

@app.route("/",methods = ['POST','GET'])
def index():
    global frec
    global servo1
    global servo2
    global servo3
    global servoSelect
    if request.method == 'POST':
        servo1 = request.values.get('servo1')
        servo2 = request.values.get('servo2')
        servo3 = request.values.get('servo3')
        servoSelect = request.values.get('servoSelect')
    return render_template('response.html', frec=frec,servo1=servo1, servo2=servo2, servo3=servo3, servoSelect=servoSelect)

if __name__ == "__main__":
    # Define HOST y PUERTO para accerder
    app.run(host='192.168.4.2', port=5000, debug=True)
