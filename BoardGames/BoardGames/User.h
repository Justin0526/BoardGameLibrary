#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
using namespace std;

class User {
protected:
    int userId;
    string name;
    string password;
    string role;

public:
    User();
    User(int id, string name, string password, string role);

    bool login(string inputName, string inputPassword) const;

    int getUserId() const;
    string getName() const;
    string getRole() const;
};

ostream& operator<<(std::ostream& os, const User& u);

#endif