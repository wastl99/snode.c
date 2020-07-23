#ifndef LEGACYWEBAPP_H
#define LEGACYWEBAPP_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "../WebApp.h"


namespace legacy {

    class WebApp : public ::WebApp {
    public:
        explicit WebApp(const std::string& rootDir)
            : ::WebApp(rootDir){};

        WebApp(const WebApp& webApp)
            : ::WebApp(webApp.getRootDir()) {
            this->setRoute(webApp.getRoute());
        }

        WebApp& operator=(const ::WebApp& webApp) = delete;

        void listen(int port, const std::function<void(int err)>& onError = nullptr) override;

    private:
        using ::WebApp::start;
        using ::WebApp::stop;
    };

} // namespace legacy

#endif // LEGACYWEBAPP_H
