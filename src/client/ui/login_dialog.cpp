#include "login_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include "../util/logger.h"

LoginDialog::LoginDialog(ClientCommunication &communication, QWidget *parent)
    : QDialog(parent), _communication(communication) {
    setWindowTitle("DeskPP - Login");
    setupUI();

    connect(_loginButton, &QPushButton::clicked, this, &LoginDialog::login);
    connect(_registerPageButton, &QPushButton::clicked, this, &LoginDialog::showRegisterPage);
    connect(_createAccountButton, &QPushButton::clicked, this, &LoginDialog::registerUser);
    connect(_loginPageButton, &QPushButton::clicked, this, &LoginDialog::showLoginPage);
}

void LoginDialog::setupUI() {
    _stackedWidget = new QStackedWidget(this);

    // Login page
    QWidget *loginPage = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);

    QFormLayout *loginForm = new QFormLayout();
    _usernameEdit = new QLineEdit();
    _passwordEdit = new QLineEdit();
    _passwordEdit->setEchoMode(QLineEdit::Password);

    loginForm->addRow("Username:", _usernameEdit);
    loginForm->addRow("Password:", _passwordEdit);

    QHBoxLayout *loginButtonLayout = new QHBoxLayout();
    _loginButton = new QPushButton("Login");
    _loginButton->setDefault(true);
    _registerPageButton = new QPushButton("Create New Account");

    loginButtonLayout->addWidget(_loginButton);
    loginButtonLayout->addWidget(_registerPageButton);

    loginLayout->addLayout(loginForm);
    loginLayout->addLayout(loginButtonLayout);

    // Register page
    QWidget *registerPage = new QWidget();
    QVBoxLayout *registerLayout = new QVBoxLayout(registerPage);

    QFormLayout *registerForm = new QFormLayout();
    _regUsernameEdit = new QLineEdit();
    _regPasswordEdit = new QLineEdit();
    _regPasswordEdit->setEchoMode(QLineEdit::Password);
    _regConfirmPasswordEdit = new QLineEdit();
    _regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    _regEmailEdit = new QLineEdit();
    _regFullNameEdit = new QLineEdit();

    registerForm->addRow("Username:", _regUsernameEdit);
    registerForm->addRow("Password:", _regPasswordEdit);
    registerForm->addRow("Confirm Password:", _regConfirmPasswordEdit);
    registerForm->addRow("Email:", _regEmailEdit);
    registerForm->addRow("Full Name:", _regFullNameEdit);

    QHBoxLayout *registerButtonLayout = new QHBoxLayout();
    _createAccountButton = new QPushButton("Create Account");
    _createAccountButton->setDefault(true);
    _loginPageButton = new QPushButton("Back to Login");

    registerButtonLayout->addWidget(_createAccountButton);
    registerButtonLayout->addWidget(_loginPageButton);

    registerLayout->addLayout(registerForm);
    registerLayout->addLayout(registerButtonLayout);

    // Add pages to stacked widget
    _stackedWidget->addWidget(loginPage);
    _stackedWidget->addWidget(registerPage);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_stackedWidget);
}

void LoginDialog::login() {
    QString username = _usernameEdit->text().trimmed();
    QString password = _passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both username and password.");
        return;
    }

    auto user = _communication.loginUser(username.toStdString(), password.toStdString());

    if (user) {
        LOG_INFO("User successfully logged in: {}", user->getUsername());
        emit userLoggedIn(*user);
        accept();
    } else {
        QMessageBox::warning(this, "Login Error", "Invalid username or password.");
    }
}

void LoginDialog::registerUser() {
    QString username = _regUsernameEdit->text().trimmed();
    QString password = _regPasswordEdit->text();
    QString confirmPassword = _regConfirmPasswordEdit->text();
    QString email = _regEmailEdit->text().trimmed();
    QString fullName = _regFullNameEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Registration Error", "Username, password, and email are required.");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Registration Error", "Passwords do not match.");
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "Registration Error", "Password must be at least 6 characters long.");
        return;
    }

    QRegularExpression emailRegex("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,}\\b");
    emailRegex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = emailRegex.match(email);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Registration Error", "Please enter a valid email address.");
        return;
    }

    auto user = _communication.registerUser(username.toStdString(), password.toStdString(),
                                         email.toStdString(), fullName.toStdString());

    if (user) {
        LOG_INFO("User successfully registered: {}", user->getUsername());
        QMessageBox::information(this, "Registration Successful",
                             "Your account has been created successfully.");
        emit userLoggedIn(*user);
        accept();
    } else {
        QMessageBox::warning(this, "Registration Error",
                          "Registration failed. Username or email may already be in use.");
    }
}

void LoginDialog::showLoginPage() {
    _stackedWidget->setCurrentIndex(0);
}

void LoginDialog::showRegisterPage() {
    _stackedWidget->setCurrentIndex(1);
}