#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    std::string e[4] = {"REQUEST_METHOD", "REQUEST_URI", "SERVER_PROTOCOL", "QUERY_STRING"};
    std::string content = {};
    content.append("<html>");
    content.append("<head>");
    content.append("<title>Envrionment Variables</title>");
    content.append("</head>");
    content.append("<body>");
    content.append("<table border = \"1\" cellspacing = \"0\">");

    for (int i = 0; i < 4; ++i)
    {
        content.append("<tr><td>" + e[i] + "</td><td>");
        const char *value = getenv(e[i].c_str());
        if (value == NULL)
            value = "Not exist";
        content.append(value);
        content.append("</td></tr>");
    }
    content.append("</table>");
    content.append("</body>");
    content.append("</html>");
    content.append("\n");
    std::cout << "Content-type:text/html\r\n";
    std::cout << "Content-Length:" << content.length() << "\r\n\r\n";
    std::cout << content;
    return 0;
}

// #include <iostream>

// int main(int argc, char *argv[])
// {
//     std::cout << "Hello World" << std::endl;

//     return 0;
// }