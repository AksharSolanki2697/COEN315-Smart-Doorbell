import os 
from flask import Flask,render_template, request, jsonify
from werkzeug.utils import secure_filename
import sqlite3 
from sqlite3 import Error
import insert
import hashlib
from passlib import hash
from passlib.hash import sha256_crypt
import select
from flask import send_from_directory


UPLOAD_FOLDER = '/var/www/Flask2/Flask2/templates/pictures/'
app = Flask(__name__)

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


@app.route("/")
def hello():
    return render_template('login.html') 

@app.route('/handle_data', methods=['POST'])
def handle_data():
    username = request.form['uname']
    password = request.form['psw']
    #return password
    db_password = select.main(username)
    if db_password:
    #return str(db_password)
         if sha256_crypt.verify(password,db_password):
            return render_template('images.html')
    return "Invalid Credentials"

@app.route("/register")
def register():
    return render_template('register.html')

@app.route('/handle_register_data', methods=['POST'])
def handle_register_data():
    username  = request.form['uname']
    password  = request.form['psw']
    rep_password = request.form['psw-repeat']
    if password == rep_password:
        new_password = sha256_crypt.encrypt(password)        
        insert.main(username,new_password)
        return render_template('login.html')
    else: 
        return "Passwords donot match"       
    return 

@app.route("/images", methods = ["GET","POST"])
def get_images():
    f  = None
    i = 0
    if request.method == "POST" :

        f = request.files['file']
        filename = ""
        for i in range(0,200):
            if not os.path.exists(UPLOAD_FOLDER + "Picture" + str(i) + ".jpg"):
                filename = "Picture" + str(i) + ".jpg"
                break

        filename = secure_filename(filename)
        f.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
        return "Files have been saved"
    else: 
        return "yo"

@app.route("/display_images")
def display_images():
    file_names =  os.listdir("/var/www/Flask2/Flask2/templates/pictures/")
    return render_template('images.html', results = file_names)


@app.route("/uploads/<filename>")
def send_file(filename):

    return send_from_directory(UPLOAD_FOLDER,filename)

@app.route("/handle_data_phonegap", methods = ["POST"] )
def handle_phonegap_login():
    username = request.form['uname']
    password = request.form['psw']
    #return password
    db_password = select.main(username)
    if db_password:
   #return str(db_password)
        if sha256_crypt.verify(password,db_password):
            return jsonify({'msg':'True'})
    return jsonify({'msg':'False'})



@app.route('/count',methods = ["GET"])
def count():
    
    list = os.listdir("/var/www/Flask2/Flask2/templates/pictures/")
    number_files = len(list)
    return jsonify({'count': number_files})

if __name__ == "__main__":
    app.run()



