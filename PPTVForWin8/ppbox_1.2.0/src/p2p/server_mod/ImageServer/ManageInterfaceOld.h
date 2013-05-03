// ManageInterfaceOld.h

#ifndef _MANAGEINTERFACEOLD_H_
#define _MANAGEINTERFACEOLD_H_

class ImageManageServerOld;

class ManageInterfaceOld
{
public:
    static Util::ErrorCode init(
        Util::Config & conf);

    static Util::ErrorCode start(
        boost::asio::io_service & io_service);

    static Util::ErrorCode stop();

private:
    static ImageManageServerOld * server_;
};

#endif // #ifndef _MANAGEINTERFACEOLD_H_
