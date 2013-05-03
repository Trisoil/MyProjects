// DownloadSession.h

#ifndef _TOOLS_TEST_DOWNLOAD_SESSION_H_
#define _TOOLS_TEST_DOWNLOAD_SESSION_H_

void download_movie(
    framework::configure::Config & conf, 
    InputHandler & std_in, 
    std::string const & name, 
    std::string const & format, 
    std::string const & path);

#endif // _TOOLS_TEST_DOWNLOAD_SESSION_H_
