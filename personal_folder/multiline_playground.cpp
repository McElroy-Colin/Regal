#include <fstream>
#include <sstream>
#include <filesystem>
#include "inc_langdef/lexer.hpp"
#include "inc_langdef/parser.hpp"
#include "inc_langdef/optimizer.hpp"

using std::string, std::list, std::map, std::ostringstream, std::ifstream, std::runtime_error;
using namespace std::filesystem;
using namespace TokenDef;
using namespace CodeTree;


void read_file(string input_file, string& file_contents) {
    ifstream inputFile(current_path().string() + "/personal_folder/" + input_file);

    if (!inputFile.is_open()) {
        throw runtime_error("Could not open the file!");
    }

    ostringstream buffer;
    buffer << inputFile.rdbuf();

    file_contents = buffer.str();

    return;
}

void interpret_file(string& file_contents) {
    list<token> token_list;
    syntaxNode parsed_code;
    map<string, syntaxNode> stack;

    lex_string(file_contents, token_list);
    parsed_code = parse_file(token_list);
    optimize_action(parsed_code, stack);

    return;
}

int main() {
    string code;

    read_file("nutsCODE.txt", code);
    interpret_file(code);

    return 0;
}