from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField
from wtforms.validators import DataRequired, Length, Email, EqualTo, ValidationError
from app.models import User


class RegistreForm(FlaskForm):
    username = StringField('Username',
                           validators=[DataRequired(),
                                       Length(min=2, max=20)])
    email = StringField('Email', validators=[DataRequired(), Email()])
    password = PasswordField('Password', validators=[DataRequired()])
    confirmPassword = PasswordField(
        'Confirm Password', validators=[DataRequired(),
                                        EqualTo('password')])
    submit = SubmitField('Sign Up')

    def validate_username(self, username):
        user = User.query.filter_by(username=username.data).first()
        print(user)
        if user:
            raise ValidationError(
                ' That username already used . Take an other username')

    def validate_email(self, email):
        user = User.query.filter_by(email=email.data).first()
        if user:
            raise ValidationError(
                ' That Email already used . Take an other email')


class LoginForm(FlaskForm):
    email = StringField('Email',
                        validators=[DataRequired(), Email()],
                        render_kw={"placeholder": "name@example.com"})
    password = PasswordField('Password',
                             validators=[DataRequired()],
                             render_kw={"placeholder": "Password"})
    rememberme = BooleanField('Remember me')
    submit = SubmitField('Log In')


# class ByForm(FlaskForm):
#     id = StringField()
#     submit = SubmitField('<i class="fa-solid fa-store"></i>')
