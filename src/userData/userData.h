#include <vector>
#include <Arduino.h>

 // TODO prevent buffer overflows

class UserData{
private:
    typedef struct {
        char* info;
        char* email;
        char* userName;
        uint32_t timestamp;
    }userData;

    std::vector<userData> userMessages;

public:
    void addUser(char* info ,char* email, char* userName, uint32_t timestamp );
    UserData(/* args */);
    ~UserData();
};
