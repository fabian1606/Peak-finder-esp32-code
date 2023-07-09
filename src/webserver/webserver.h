#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>

class Webserver
{
public:
    typedef void (*addMessageCallback_t)(String info, String email, String userName, uint32_t timestamp);
    addMessageCallback_t* addMessageCallback ;
    Webserver();
    void init(IPAddress * ip);
    void update();
    void setAddMessageCallback(addMessageCallback_t* callback);
private:
    const char* configHtml = R"pogremm(<!DOCTYPE html> <html lang="en"> <head> <style> *{margin: 0; padding: 0; box-sizing: border-box; font-family: Arial, Helvetica, sans-serif;} li{list-style: none; margin: 10px; margin-left:0px ;margin-right: 40px; padding: 10px; background-color: white; border-radius: 15px; border-bottom-left-radius: 0;} li.user-message{ border-bottom-left-radius: 15px; border-bottom-right-radius: 0; margin-left:40px ;margin-right: 0px;} .open{height: 50vh !important ;} .open_text{display: block !important;} textarea{width: 100%; border-radius: 15px; padding: 5px; margin-bottom: 15px; outline:  none;} .rotate{transform: rotate(180deg);} </style> <meta charset="UTF-8"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>Document</title> </head> <body style="background-color: lightblue;"> <div style=" margin: 30px; margin-top: 0px; margin-bottom: 30px; padding: 20px; height: 100vh;"> <div style="display: flex; justify-content: center; position: fixed; background-color: #242227; width: 100%; height: 60px ; top: 0; left: 0; padding: 10px;"> <img src="logo.png" alt="Logo" style="width: 40px;"> <h2 style="margin-top: 7px; margin-left: 5px; color: #FBDF96;">Peakfinder</h2> </div> <ul id="message_list" style="margin-top: 50px; margin-bottom: 60px;"> </ul> </div> <div id="drawer" style="display: flex; flex-direction: column; align-items: center; background-color: #242227; position: fixed; bottom: 0; height: 7vh; width: 100%;"> <button id="iconButtom" style="background-color: #24222700; border: 0;"><img id="chevron" src="chevron.png" alt="open drawer"></button> <div id="text_areas" style="display: none; padding: 5px;"> <div style="display: flex; flex-direction: column;"> <p style="color: white;">Your Email</p> <textarea class="drawer_text" name="email" id="email" cols="30" rows="1" placeholder="email"></textarea> <p style="color: white;">Your Name</p> <textarea class="drawer_text" name="name" id="name" cols="30" rows="1" placeholder="your name"></textarea> <p style="color: white;">Your Message</p> <textarea class="drawer_text" name="text" id="text" cols="30" rows="3" placeholder="your message"></textarea> </div> <button id="entry" style="float: right; height: 25px; width: 60px; border: 0; border-radius: 10px;">entry</button> </div> </div> </body> </html> <script> document.getElementById("iconButtom").addEventListener("click", function(){ document.getElementById("drawer").classList.toggle("open"); document.getElementById("text_areas").classList.toggle("open_text"); document.getElementById("chevron").classList.toggle("rotate"); }); function loadMessages(){ document.getElementById("message_list").innerHTML = JSON.messages.map(message => { return `<li class="${message.name == Name ? "user-message" : "message"}"> <p style="display:inline">${message.name}</p> <p style="display:inline">${message.created_at}</p> <p>${message.message}</p> </li>` }).join("");} loadMessages(); if(localStorage.getItem("used") == null){ localStorage.setItem("used", false); } //     document.getElementById("entry").addEventListener("click", function (){ // if(localStorage.getItem("used") == "false"){ //     localStorage.setItem("used", true); //             const email = document.getElementById("email").value; //             const name = document.getElementById("name").value; //             const text = document.getElementById("text").value; //             const message = { //                 "id": 8, //                 "created_at": "2020-06-01", //                 "name": name, //                 "email": email, //                 "message": text //             } //             JSON.messages.unshift(message); //             loadMessages(); //         } //     }); </script>)pogremm";
    WebServer server;
    DNSServer dnsServer;
};
