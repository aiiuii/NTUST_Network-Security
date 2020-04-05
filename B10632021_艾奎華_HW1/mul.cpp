#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    std::string content = {};
    char *data;
    long m, n;

    content.append("<html>");
    content.append("<head>");
    content.append("<title>Multiplication results</title>");
    content.append("</head>");
    content.append("<body>");

    data = getenv("QUERY_STRING");
    if (data == NULL)
        content.append("<P>Error! Error in passing data from form to script.");
    else if (sscanf(data, "m=%ld&n=%ld", &m, &n) != 2)
        content.append("<P>Error! Invaild data. Data must be numeric.");
    else
    {
        content.append("<P>The product of " + std::to_string(m) + " and " + std::to_string(n) + " is " + std::to_string(m * n) + ".");
    }

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