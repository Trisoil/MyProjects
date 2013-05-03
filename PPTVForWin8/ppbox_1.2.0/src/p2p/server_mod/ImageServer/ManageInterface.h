// ManageInterface.h

#ifndef _MANAGEINTERFACE_H_
#define _MANAGEINTERFACE_H_

class ImageManageServer;

class ManageInterface
{
public:
    static Util::ErrorCode init(
        Util::Config & conf);

    static Util::ErrorCode start(
        boost::asio::io_service & io_service);

    static Util::ErrorCode stop();

private:
    static ImageManageServer * server_;
};

#endif // #ifndef _MANAGEINTERFACE_H_
