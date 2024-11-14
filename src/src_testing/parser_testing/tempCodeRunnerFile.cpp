TokenList lexed2 = lex_string(line);

    auto new_start = std::chrono::high_resolution_clock::now();
    parse_line_c(lexed2);
    auto new_end = std::chrono::high_resolution_clock::now();

    auto new_length = std::chrono::duration_cast<std::chrono::nanoseconds>(new_end - new_start).count();