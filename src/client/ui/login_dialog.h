#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include "../net/api_client.h"

/**
 * @class LoginDialog
 * @brief Dialog logowania i rejestracji użytkownika.
 *
 * Umożliwia logowanie istniejących użytkowników oraz rejestrację nowych kont.
 * Wykorzystuje interfejs z zakładkami do przełączania między logowaniem i rejestracją.
 */
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param apiClient Referencja do klienta API
     * @param parent Obiekt rodzica (opcjonalny)
     */
    explicit LoginDialog(ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    /**
     * @brief Obsługuje logowanie użytkownika
     */
    void login();

    /**
     * @brief Obsługuje rejestrację nowego użytkownika
     */
    void registerUser();

    /**
     * @brief Przełącza na widok rejestracji
     */
    void switchToRegister();

    /**
     * @brief Przełącza na widok logowania
     */
    void switchToLogin();

private:
    ApiClient &apiClient;
    QStackedWidget *stack;

    // Strona logowania
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    // Strona rejestracji
    QLineEdit *regUsernameEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regEmailEdit;
};

#endif
