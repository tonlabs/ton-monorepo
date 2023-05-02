#include "extwords.h"
#include "IntCtx.h"

namespace fift {

void interpret_shell(vm::Stack& stack, bool err_to_out) {
    std::array<char, 128> buffer;
    std::string result;

    auto cmd = stack.pop_string();
    if (err_to_out) cmd += " 2>&1";

    auto pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw IntError{"popen failed while executing"};

    while (!feof(pipe)) {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
    }

    auto exit_code = pclose(pipe) >> 8;

    stack.push_string(result);
    stack.push_smallint(exit_code);
}

void init_words_ext(Dictionary& d, bool allow_shell) {
    using namespace std::placeholders;

    if (allow_shell) {
        d.def_stack_word("shell ", std::bind(interpret_shell, _1, false));
        d.def_stack_word("shell2>&1 ", std::bind(interpret_shell, _1, true));
    }
}

} // namespace fift
