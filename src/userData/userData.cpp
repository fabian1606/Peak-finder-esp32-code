#include "userData.h"

UserData::UserData(){}

void UserData::addUser(char* info ,char* email, char* userName, uint32_t timestamp ){
    userData newMessage;
    newMessage.email = email;
    newMessage.info = info;
    newMessage.userName = userName;
    userMessages.push_back(newMessage);
}