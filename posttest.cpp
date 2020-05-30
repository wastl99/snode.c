#include <iostream>
#include <string.h>
#include <time.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "ContinousTimer.h"
#include "SingleshotTimer.h"
#include "WebApp.h"

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


int testPost(int argc, char* argv[]) {
    WebApp app("/home/voc/projects/ServerVoc/build/html");

    app.get("/", [&](const Request& req, const Response& res) -> void {
        res.send("<html>"
                    "<head>"
                        "<style>"
                            "main {"
                                "min-height: 30em;"
                                "padding: 3em;"
                                "background-image: repeating-radial-gradient( circle at 0 0, #fff, #ddd 50px);"
                            "}"
                            "input[type=\"file\"] {"
                                "display: block;"
                                "margin: 2em;"
                                "padding: 2em;"
                                "border: 1px dotted;"
                            "}"
                        "</style>"
                    "</head>"
                    "<body>"
                        "<h1>Datei-Upload mit input type=\"file\"</h1>"
                        "<main>"
                            "<h2>Schicken Sie uns was Schickes!</h2>"
                            "<form method=\"post\" enctype=\"multipart/form-data\">"
                                "<label> Wählen Sie eine Textdatei (*.txt, *.html usw.) von Ihrem Rechner aus."
                                    "<input name=\"datei\" type=\"file\" size=\"50\" accept=\"text/*\">"
                                "</label>"
                                "<button>… und ab geht die Post!</button>"
                            "</form>"
                        "</main>"
                    "</body>"
                 "</html>");
    });

    app.post("/", [&](const Request& req, const Response& res) -> void {
        std::cout << "Content-Type: " << req.header("Content-Type") << std::endl;
        std::cout << "Content-Length: " << req.header("Content-Length") << std::endl;
        char* body = new char[std::stoul(req.header("Content-Length")) + 1];
        memcpy(body, req.body, std::stoul(req.header("Content-Length")));
        body[std::stoi(req.header("Content-Length"))] = 0;

        std::cout << "Body: " << std::endl;
        std::cout << body << std::endl;
        res.send("<html>"
                    "<body>"
                        "<h1>Thank you</h1>"
                    "</body>"
                "</html>");
    });

#define CERTF "/home/voc/projects/ServerVoc/certs/calisto.home.vchrist.at_-_snode.c.pem"
#define KEYF "/home/voc/projects/ServerVoc/certs/Volker_Christian_-_Web_-_snode.c.key.encrypted.pem"
#define KEYFPASS "snode.c"

    app.listen(8080, [&app](int err) -> void {
        if (err != 0) {
            perror("Listen");
        } else {
            std::cout << "snode.c listening on port 8080" << std::endl;
        }
    });

    app.sslListen(8088, CERTF, KEYF, KEYFPASS, [](int err) -> void {
        if (err != 0) {
            perror("Listen");
        } else {
            std::cout << "snode.c listening on port 8088" << std::endl;
        }
    });
    
    WebApp::start();

    return 0;
}


int main(int argc, char** argv) {
    return testPost(argc, argv);
}
