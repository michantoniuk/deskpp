#include "login_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), apiClient(apiClient) {
    setWindowTitle("DeskPP - Login");
    setMinimumWidth(300);

    stack = new QStackedWidget(this);

    // Login page
    auto loginPage = new QWidget();
    auto loginLayout = new QVBoxLayout(loginPage);

    auto loginForm = new QFormLayout();
    usernameEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginForm->addRow("Username:", usernameEdit);
    loginForm->addRow("Password:", passwordEdit);
    loginLayout->addLayout(loginForm);

    auto loginBtnLayout = new QHBoxLayout();

    auto loginBtn = new QPushButton("Login", this);
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::login);
    loginBtnLayout->addWidget(loginBtn);

    auto registerBtn = new QPushButton("Create Account", this);
    connect(registerBtn, &QPushButton::clicked, this, &LoginDialog::switchToRegister);
    loginBtnLayout->addWidget(registerBtn);

    loginLayout->addLayout(loginBtnLayout);

    // Register page
    auto registerPage = new QWidget();
    auto registerLayout = new QVBoxLayout(registerPage);

    auto registerForm = new QFormLayout();
    regUsernameEdit = new QLineEdit();
    regPasswordEdit = new QLineEdit();
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regEmailEdit = new QLineEdit();

    registerForm->addRow("Username:", regUsernameEdit);
    registerForm->addRow("Password:", regPasswordEdit);
    registerForm->addRow("Email:", regEmailEdit);
    registerLayout->addLayout(registerForm);

    auto regBtnLayout = new QHBoxLayout();

    auto createBtn = new QPushButton("Create Account", this);
    connect(createBtn, &QPushButton::clicked, this, &LoginDialog::registerUser);
    regBtnLayout->addWidget(createBtn);

    auto backBtn = new QPushButton("Back to Login", this);
    connect(backBtn, &QPushButton::clicked, this, &LoginDialog::switchToLogin);
    regBtnLayout->addWidget(backBtn);

    registerLayout->addLayout(regBtnLayout);

    // Add pages to stack
    stack->addWidget(loginPage);
    stack->addWidget(registerPage);

    // Main layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stack);
}

void LoginDialog::login() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password");
        return;
    }

    auto user = apiClient.loginUser(username.toStdString(), password.toStdString());

    if (user) {
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Login failed. Please check your credentials.");
    }
}

void LoginDialog::registerUser() {
    QString username = regUsernameEdit->text();
    QString password = regPasswordEdit->text();
    QString email = regEmailEdit->text();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Error", "All fields are required");
        return;
    }

    auto user = apiClient.registerUser(username.toStdString(), password.toStdString(),
                                       email.toStdString());

    if (user) {
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Registration failed. Username may be taken.");
    }
}

void LoginDialog::switchToRegister() {
    stack->setCurrentIndex(1);
}

void LoginDialog::switchToLogin() {
    stack->setCurrentIndex(0);
}
