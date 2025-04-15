#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QRegularExpression>
#include "../net/api_client.h"
#include "../model/models.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(ApiClient &apiClient, QWidget *parent = nullptr);

signals:
    void userLoggedIn(const User &user);

private slots:
    void login();

    void registerUser();

    void showLoginPage();

    void showRegisterPage();

private:
    void setupUI();

    ApiClient &_apiClient;
    QStackedWidget *_stackedWidget;

    // Login page widgets
    QLineEdit *_usernameEdit;
    QLineEdit *_passwordEdit;
    QPushButton *_loginButton;
    QPushButton *_registerPageButton;

    // Register page widgets
    QLineEdit *_regUsernameEdit;
    QLineEdit *_regPasswordEdit;
    QLineEdit *_regConfirmPasswordEdit;
    QLineEdit *_regEmailEdit;
    QLineEdit *_regFullNameEdit;
    QPushButton *_createAccountButton;
    QPushButton *_loginPageButton;
};

#endif // LOGIN_DIALOG_H
