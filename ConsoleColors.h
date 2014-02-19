/***
 * consoleColors.h
 *
 *  Created on: Aug 14, 2011
 *      Author: moloch
 */

#ifndef CONSOLECOLORS_H_
#define CONSOLECOLORS_H_

#include <string>

#ifndef __WIN32__

    /* Updates */
    const std::string INFO = "\033[1m\033[36m[*]\033[0m ";
    const std::string WARN = "\033[1m\033[31m[!]\033[0m ";
    const std::string DEBUG = "\033[1m\033[35m[-]\033[0m ";
    const std::string SUCCESS = "\033[1m\033[33m[$]\033[0m ";
    const std::string PROMPT = "\033[1m\033[34m[?]\033[0m ";

    /* Colors */
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string PURPLE = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string GRAY = "\033[37m";

    /* Styles */
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
    const std::string RESET = "\033[0m";
    const std::string CLEAR = "\r\x1b[2K";

#else

    /* Updates */
    const std::string INFO = "[*] ";
    const std::string WARN = "[!] ";
    const std::string DEBUG = "[-] ";
    const std::string SUCCESS = "[$] ";
    const std::string PROMPT = "[?] ";

    /* Colors */
    const std::string BLACK = "";
    const std::string RED = "";
    const std::string GREEN = "";
    const std::string YELLOW = "";
    const std::string BLUE = "";
    const std::string PURPLE = "";
    const std::string CYAN = "";
    const std::string GRAY = "";

    /* Styles */
    const std::string BOLD = "";
    const std::string UNDERLINE = "";
    const std::string RESET = "";
    const std::string CLEAR = "\n";

#endif /* __WIN32__ */

#endif /* CONSOLECOLORS_H_ */
