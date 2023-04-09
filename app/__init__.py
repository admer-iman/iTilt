from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_bcrypt import Bcrypt
from flask_login import LoginManager


db = SQLAlchemy()
app = Flask(__name__, instance_relative_config=True)
bcrypt = Bcrypt(app)
login_manager = LoginManager(app)
login_manager.login_view = 'login'

app.config['SECRET_KEY'] = 'Faty'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///store.db'
db.init_app(app)
