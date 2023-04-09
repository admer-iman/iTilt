from flask_sqlalchemy import SQLAlchemy
from app import db, login_manager
from flask_login import UserMixin


@login_manager.user_loader
def loadUser(userID):
    return User.query.get(int(userID))


class User(db.Model, UserMixin):
    id = db.Column(db.Integer(), primary_key=True)
    username = db.Column(db.String(30), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    password = db.Column(db.String(60), nullable=False)

    def __repr__(self):
        return f" {self.id} , {self.username}"


# class Commande(db.Model):
#     id = db.Column(db.Integer(), primary_key=True)
#     name = db.Column(db.String(30), nullable=False)
#     price = db.Column(db.Integer(), nullable=False)
#     clothes_type = db.Column(db.String(10), nullable=False)
#     user_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)

#     def __repr__(self):
#         return f" {self.id} ,{self.name}"


# class Product(db.Model):
#     id = db.Column(db.Integer(), primary_key=True)
#     name = db.Column(db.String(30), nullable=False)
#     price = db.Column(db.Integer(), nullable=False)
#     clothes_type = db.Column(db.String(10), nullable=False)
#     image = db.Column(db.String(50), nullable=False)
#     quantity = db.Column(db.Integer(), nullable=False)

#     def __repr__(self):
#         return f" {self.id} ,{self.name}"
