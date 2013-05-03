//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_ERROR_CODE_H
#define SUPER_NODE_ERROR_CODE_H

namespace super_node
{
    class ErrorCode
    {
        friend class ErrorCodes;
    public:
        ErrorCode()
            : error_code_(0)
        {}

        bool operator==(const ErrorCode& error_code) const
        {
            return error_code_ == error_code.error_code_;
        }

        operator int() const
        {
            return error_code_;
        }

    protected:
        ErrorCode(int error_code)
            : error_code_(error_code)
        {}

    private:
        boost::uint32_t error_code_;
    };

    class ErrorCodes
    {
    public:
        static const ErrorCode Success;
        static const ErrorCode UnknownError;
        static const ErrorCode ResourceNotFound;
        static const ErrorCode ServiceBusy;
        static const ErrorCode ServiceStopped;

        static const ErrorCode DiskWriteFailure;
        static const ErrorCode DiskReadFailure;
        static const ErrorCode DiskReadWriteFailure;

        static const ErrorCode OutOfMemory;

        static const ErrorCode DownloadFailure;
    };
}

#endif //SUPER_NODE_ERROR_CODE_H
