#ifndef CONSOLECOLORS_H_
#define CONSOLECOLORS_H_

#include <string>

#ifndef __WIN32__

    /* Updates */
    static const std::string INFO = "\033[1m\033[36m[*]\033[0m ";
    static const std::string WARN = "\033[1m\033[31m[!]\033[0m ";
    static const std::string DEBUG = "\033[1m\033[35m[-]\033[0m ";
    static const std::string SUCCESS = "\033[1m\033[33m[$]\033[0m ";
    static const std::string PROMPT = "\033[1m\033[34m[?]\033[0m ";

    /* Colors */
    static const std::string BLACK = "\033[30m";
    static const std::string RED = "\033[31m";
    static const std::string GREEN = "\033[32m";
    static const std::string YELLOW = "\033[33m";
    static const std::string BLUE = "\033[34m";
    static const std::string PURPLE = "\033[35m";
    static const std::string CYAN = "\033[36m";
    static const std::string GRAY = "\033[37m";

    /* Styles */
    static const std::string BOLD = "\033[1m";
    static const std::string UNDERLINE = "\033[4m";
    static const std::string RESET = "\033[0m";
    static const std::string CLEAR = "\r\x1b[2K";

#else

    /* Updates */
    static const std::string INFO = "[*] ";
    static const std::string WARN = "[!] ";
    static const std::string DEBUG = "[-] ";
    static const std::string SUCCESS = "[$] ";
    static const std::string PROMPT = "[?] ";

    /* Windows Color Support is Terrible :( */
    static const std::string BLACK = "";
    static const std::string RED = "";
    static const std::string GREEN = "";
    static const std::string YELLOW = "";
    static const std::string BLUE = "";
    static const std::string PURPLE = "";
    static const std::string CYAN = "";
    static const std::string GRAY = "";

    /* Styles Blank */
    static const std::string BOLD = "";
    static const std::string UNDERLINE = "";
    static const std::string RESET = "";
    static const std::string CLEAR = "\n";

#endif /* __WIN32__ */

#endif /* CONSOLECOLORS_H_ */
