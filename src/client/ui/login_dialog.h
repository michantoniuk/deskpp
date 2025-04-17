#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include "../net/api_client.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    void login();

    void registerUser();

    void switchToRegister();

    void switchToLogin();

private:
    ApiClient &apiClient;
    QStackedWidget *stack;

    // Login page
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    // Register page
    QLineEdit *regUsernameEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regEmailEdit;
    QLineEdit *regNameEdit;
};

#endif // LOGIN_DIALOG_H
