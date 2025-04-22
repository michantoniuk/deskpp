#include "login_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), apiClient(apiClient) {
    setWindowTitle("DeskPP - Logowanie");
    setMinimumWidth(300);

    stack = new QStackedWidget(this);

    // Strona logowania
    auto loginPage = new QWidget();
    auto loginLayout = new QVBoxLayout(loginPage);

    auto loginForm = new QFormLayout();
    usernameEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginForm->addRow("Nazwa użytkownika:", usernameEdit);
    loginForm->addRow("Hasło:", passwordEdit);
    loginLayout->addLayout(loginForm);

    auto loginBtnLayout = new QHBoxLayout();

    auto loginBtn = new QPushButton("Zaloguj", this);
    loginBtn->setDefault(true);
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::login);
    loginBtnLayout->addWidget(loginBtn);

    auto registerBtn = new QPushButton("Utwórz konto", this);
    connect(registerBtn, &QPushButton::clicked, this, &LoginDialog::switchToRegister);
    loginBtnLayout->addWidget(registerBtn);

    loginLayout->addLayout(loginBtnLayout);

    // Strona rejestracji
    auto registerPage = new QWidget();
    auto registerLayout = new QVBoxLayout(registerPage);

    auto registerForm = new QFormLayout();
    regUsernameEdit = new QLineEdit();
    regPasswordEdit = new QLineEdit();
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regEmailEdit = new QLineEdit();

    registerForm->addRow("Nazwa użytkownika:", regUsernameEdit);
    registerForm->addRow("Hasło:", regPasswordEdit);
    registerForm->addRow("Email:", regEmailEdit);
    registerLayout->addLayout(registerForm);

    auto regBtnLayout = new QHBoxLayout();

    auto createBtn = new QPushButton("Utwórz konto", this);
    connect(createBtn, &QPushButton::clicked, this, &LoginDialog::registerUser);
    regBtnLayout->addWidget(createBtn);

    auto backBtn = new QPushButton("Powrót do logowania", this);
    connect(backBtn, &QPushButton::clicked, this, &LoginDialog::switchToLogin);
    regBtnLayout->addWidget(backBtn);

    registerLayout->addLayout(regBtnLayout);

    // Dodaj strony do stosu
    stack->addWidget(loginPage);
    stack->addWidget(registerPage);

    // Główny układ
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stack);
}

void LoginDialog::login() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Wprowadź nazwę użytkownika i hasło");
        return;
    }

    auto user = apiClient.loginUser(username.toStdString(), password.toStdString());

    if (user) {
        accept();
    } else {
        QMessageBox::warning(this, "Błąd", "Logowanie nie powiodło się. Sprawdź dane logowania.");
    }
}

void LoginDialog::registerUser() {
    QString username = regUsernameEdit->text();
    QString password = regPasswordEdit->text();
    QString email = regEmailEdit->text();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Wszystkie pola są wymagane");
        return;
    }

    auto user = apiClient.registerUser(username.toStdString(), password.toStdString(),
                                       email.toStdString());

    if (user) {
        accept();
    } else {
        QMessageBox::warning(this, "Błąd", "Rejestracja nie powiodła się. Nazwa użytkownika może być zajęta.");
    }
}

void LoginDialog::switchToRegister() {
    stack->setCurrentIndex(1);
}

void LoginDialog::switchToLogin() {
    stack->setCurrentIndex(0);
}
