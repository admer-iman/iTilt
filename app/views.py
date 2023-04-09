from app import app, db, bcrypt
from app.forms import RegistreForm, LoginForm
from flask import render_template, redirect, flash, url_for, request, make_response, jsonify
from app.models import User
from flask_login import login_user, current_user, logout_user, login_required
from werkzeug.datastructures import ImmutableMultiDict

data = {
    "tilt" : None ,
    "roll" : None ,
    "grav" : None ,
    "abv" : None ,
    "tem": None,
    "intemp" : None ,
}

@app.route("/send")
def send():
    global data
    # get data from the url variable
    data = request.args.to_dict(flat=True)
    # print(data)
    return "done"


@app.route("/index", methods=['GET', 'POST'])
@app.route("/", methods=['GET', 'POST'])
@login_required
def home():
    # redirect the user if try to enter to home page in he not login 
    if not current_user.is_authenticated:
        print(current_user.is_authenticated)
        return redirect('login')

# response to the POST method send by js script
    if request.method == 'POST':
        # print(data.get("tem"))
        # print(type(data.get("tem")))
        print(data)
        return make_response(jsonify(data), 200)
    return render_template("home.html")


@app.route("/register", methods=["POST", "GET"])
def register():
    form = RegistreForm()
    if current_user.is_authenticated:
        return redirect('/')
    if (form.validate_on_submit()):
        # store in db
        hashed_pswd = bcrypt.generate_password_hash(
            form.password.data).decode('utf-8')
        user = User(username=form.username.data,
                    email=form.email.data,
                    password=hashed_pswd)
        db.session.add(user)
        db.session.commit()
        # send a flash message for notificate create a accountw
        flash(f' Account created for {form.username.data} ', 'success')
        return redirect('/')
    return render_template("register.html", form=form)


@app.route("/login", methods=["POST", "GET"])
def login():
    form = LoginForm()
    if current_user.is_authenticated:
        return redirect('/')
    if (form.validate_on_submit()):
        user = User.query.filter_by(email=form.email.data).first()
        if user and bcrypt.check_password_hash(user.password,
                                               form.password.data):
            login_user(user, remember=form.rememberme.data)
            flash('You have been logged in!', 'success')
            return redirect('/')
        else:
            flash('Login Unsuccessful', 'danger')

    return render_template("login.html", form=form)


@app.route("/logout")
def logout():
    logout_user()
    return redirect('login')
