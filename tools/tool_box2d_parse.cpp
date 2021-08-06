/*
This program is meant to extract the method signatures and class names from box2d files
It is expected that the "strip c comments" program has already been run as this program
fails to understand comments and method bodies
*/

#define COLOR_RESET   "\033[0m"
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <tuple>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

enum class TT {
	ERROR, END, PUBLIC, PRIVATE, INTERNAL, STATIC, CLASS, EXTENDS, FUNCTION, VAR, CONST, ID, LPAREN, RPAREN, COLON, COMMA, EQUALS, OVERRIDE, VIRTUAL, GET, SET
};

struct Token {
	const TT kind;
	const std::string id;
	Token(TT t)
		: kind(t), id() {}
	explicit Token(const std::string &s)
		: kind(TT::ID), id(s) {}
};

using TokenVec = std::vector<Token>;

static bool valid_id_char(const char c)
{
	if (std::isalnum(c))
		return true;
	const char *other_chars = "_$.*<>";
	for ( ; *other_chars != '\0'; ++other_chars) {
		if (c == *other_chars)
			return true;
	}
	return false;
}

static Token parse_chunk(std::string &chunk)
{
	const std::pair<char, Token> char_lookup[] {
		{ '(', TT::LPAREN },
		{ ')', TT::RPAREN },
		{ ':', TT::COLON },
		{ ',', TT::COMMA },
		{ '=', TT::EQUALS }
	};
	constexpr int char_lookup_len = ARR_SIZE(char_lookup);
	const std::pair<std::string, Token> string_lookup[] {
		{ "public", TT::PUBLIC },
		{ "private", TT::PRIVATE },
		{ "internal", TT::INTERNAL },
		{ "static", TT::STATIC },
		{ "class", TT::CLASS },
		{ "extends", TT::EXTENDS },
		{ "var", TT::VAR },
		{ "const", TT::CONST },
		{ "function", TT::FUNCTION },
		{ "override", TT::OVERRIDE },
		{ "virtual", TT::VIRTUAL },
		{ "get", TT::GET },
		{ "set", TT::SET },
	};
	constexpr int string_lookup_len = ARR_SIZE(string_lookup);
	for (int i = 0; i < char_lookup_len; ++i) {
		if (chunk[0] == char_lookup[i].first) {
			chunk = chunk.substr(1);
			return char_lookup[i].second;
		}
	}
	for (int i = 0; i < string_lookup_len; ++i) {
		if (chunk == string_lookup[i].first) {
			chunk.clear();
			return string_lookup[i].second;
		}
	}
	if (! valid_id_char(chunk[0]))
		return TT::ERROR;
	std::string id;
	size_t i;
	for (i = 0; i < chunk.size() && valid_id_char(chunk[i]); ++i) {
		id += chunk[i];
	}
	if (i == chunk.size())
		chunk.clear();
	else
		chunk = chunk.substr(i);
	return Token(id);
}

static void multi_parse_chunk(std::string &chunk, TokenVec &vec)
{
	while (! chunk.empty()) {
		Token token = parse_chunk(chunk);
		if (token.kind == TT::ERROR)
			return;
		//std::cout << static_cast<int>(token.kind) << std::endl;
		vec.emplace_back(token);
	}
}

#define MATCH(what) switch(what)
#define WHEN(what) break; case what:
#define WHEN_EXPECT(what, boolean_expr) WHEN(what) if (! (boolean_expr)) return "expected " #boolean_expr " in step " #what ; else
#define OTHERWISE break; default:

using MethodMatch = std::tuple<std::string, std::vector<std::string>, std::string>;
using ClassMatch = std::tuple<std::string, std::string, MethodMatch, int, int, std::vector<std::string>, std::vector<std::string>>;

#define EXPECTED_METHOD_NAME(mm) std::get<0>(mm)
#define PARAM_TYPE_NAMES(mm) std::get<1>(mm)
#define RETURN_TYPE(mm) std::get<2>(mm)

#define EXPECTED_CLASS_NAME(cm) std::get<0>(cm)
#define SUPER_CLASS_NAME(cm) std::get<1>(cm)
#define CONSTRUCTOR_MATCH(cm) std::get<2>(cm)
#define NUM_STATIC_METHODS(cm) std::get<3>(cm)
#define NUM_METHODS(cm) std::get<4>(cm)
#define STATIC_VAR_TYPES(cm) std::get<5>(cm)
#define VAR_TYPES(cm) std::get<6>(cm)

static std::ostream &print_string(std::ostream &o, const std::string &s)
{
	o << '"' << s << '"';
	return o;
}

static std::ostream &print_string_vec(std::ostream &o, const std::vector<std::string> &vec)
{
	if (vec.empty()) {
		o << "{}";
	} else {
		o << "{ ";
		print_string(o, vec[0]);
		for (size_t i = 1; i < vec.size(); ++i) {
			o << ", ";
			print_string(o, vec[i]);
		}
		o << "}";
	}
	return o;
}

static void print_class_match(const ClassMatch &cm)
{
	const MethodMatch &mm = CONSTRUCTOR_MATCH(cm);
	const std::vector<std::string> ctor_params = PARAM_TYPE_NAMES(mm);
	std::cout << "{ ";
	print_string(std::cout, EXPECTED_CLASS_NAME(cm)) << ", ";
	print_string(std::cout, SUPER_CLASS_NAME(cm)) << ", ";
	std::cout << "{ ";
	print_string(std::cout, EXPECTED_METHOD_NAME(mm)) << ", ";
	print_string_vec(std::cout, ctor_params) << ", ";
	print_string(std::cout, RETURN_TYPE(mm)) << "}, ";
	std::cout << NUM_STATIC_METHODS(cm) << ", ";
	std::cout << NUM_METHODS(cm) << ", ";
	print_string_vec(std::cout, STATIC_VAR_TYPES(cm)) << ", ";
	print_string_vec(std::cout, VAR_TYPES(cm));
	std::cout << " }," << std::endl;
}

/*
	public class b2Mat22
	public function b2Mat22(angle:Number=0, c1:b2Vec2=null, c2:b2Vec2=null)

	step  0: "" | STATIC
	step  1: PRIVATE | PUBLIC | INTERNAL | ""
	step  2: "" | STATIC
	step 20: "" | OVERRIDE | VIRTUAL
	step  3: CLASS | FUNCTION | VAR | CONST
	step  4: class_name
	step 21: "" | get | set
	step  5: function_name
	step  6: LPAREN
	step  7: "" | param_name
	step  8: COLON
	step  9: type_name
	step 10: "" | EQUALS
	step 11: value
	step 12: "" | COMMA
	step 13: RPAREN
	step 14: "" | COLON
	step 15: return_type
	step 16: ~end function def
	step 17: "" | EXTENDS
	step 18: base_class
	step 19: var_name
	step 22: ~end class def
	step 23: COLON
	step 24: var_type_name
	step 25: ~end var/const def
*/

static const char *evaluate(const TokenVec &tokens)
{
	int step = 0;
	bool is_static = false;
	static ClassMatch cm;
	static MethodMatch current_mm;
	for (size_t i = 0; i < tokens.size(); ) {
		const Token &token = tokens[i];
		MATCH(step) {
		WHEN(0) {
			MATCH(token.kind) {
			WHEN(TT::STATIC) {
				is_static = true;
				++i;
			}
			OTHERWISE {
				is_static = false;
			}
			}
			step = 1;
		}
		WHEN(1) {
			MATCH(token.kind) {
			WHEN(TT::PRIVATE) {
				++i;
			}
			WHEN(TT::PUBLIC) {
				++i;
			}
			WHEN(TT::INTERNAL) {
				++i;
			}
			}
			step = 2;
			break;
		}
		WHEN(2) {
			MATCH(token.kind) {
			WHEN(TT::STATIC) {
				if (is_static) {
					return "double static";
				}
				++i;
				is_static = true;
			}
			}
			step = 20;
		}
		WHEN(20) {
			MATCH(token.kind) {
			WHEN(TT::OVERRIDE) {
				++i;
			}
			WHEN(TT::VIRTUAL) {
				++i;
			}
			}
			step = 3;
		}
		WHEN(3) {
			MATCH(token.kind) {
			WHEN(TT::CLASS) {
				++i;
				step = 4;
			}
			WHEN(TT::FUNCTION) {
				++i;
				step = 21;
			}
			WHEN(TT::VAR) {
				++i;
				step = 19;
			}
			WHEN(TT::CONST) {
				++i;
				step = 19;
			}
			OTHERWISE {
				return "expected class, function, var";
			}
			}
		}
		WHEN_EXPECT(4, token.kind == TT::ID) {
			print_class_match(cm);
			cm = ClassMatch();
			EXPECTED_CLASS_NAME(cm) = token.id;
			step = 17;
		}
		WHEN(21) {
			MATCH(token.kind) {
			WHEN(TT::GET) {
				++i;
			}
			WHEN(TT::SET) {
				++i;
			}
			}
			step = 5;
		}
		WHEN_EXPECT(5, token.kind == TT::ID) {
			current_mm = MethodMatch();
			EXPECTED_METHOD_NAME(current_mm) = token.id;
			++i;
			step = 6;
		}
		WHEN_EXPECT(6, token.kind == TT::LPAREN) {
			step = 7;
			++i;
		}
		WHEN(7) {
			MATCH(token.kind) {
			WHEN(TT::ID) {
				//param_names.emplace_back(token.id);
				step = 8;
				++i;
			}
			OTHERWISE {
				step = 13;
			}
			}
		}
		WHEN_EXPECT(8, token.kind == TT::COLON) {
			step = 9;
			++i;
		}
		WHEN_EXPECT(9, token.kind == TT::ID) {
			if (token.id == "*")
				PARAM_TYPE_NAMES(current_mm).emplace_back("");
			else
				PARAM_TYPE_NAMES(current_mm).emplace_back(token.id);
			step = 10;
			++i;
		}
		WHEN(10) {
			MATCH(token.kind) {
			WHEN(TT::EQUALS) {
				step = 11;
				++i;
			}
			OTHERWISE {
				step = 12;
			}
			}
		}
		WHEN_EXPECT(11, token.kind == TT::ID) {
			// default value... ignoring for now
			step = 12;
			++i;
		}
		WHEN(12) {
			MATCH(token.kind) {
			WHEN(TT::COMMA) {
				step = 7;
				++i;
			}
			OTHERWISE {
				step = 13;
			}
			}
		}
		WHEN_EXPECT(13, token.kind == TT::RPAREN) {
			step = 14;
			++i;
		}
		WHEN(14) {
			MATCH(token.kind) {
			WHEN(TT::COLON) {
				step = 15;
				++i;
			}
			OTHERWISE {
				//RETURN_TYPE(current_mm) = "void";
				step = 16;
			}
			}
		}
		WHEN_EXPECT(15, token.kind == TT::ID) {
			if (token.id == EXPECTED_CLASS_NAME(cm))
				RETURN_TYPE(current_mm) = "self";
			else
				RETURN_TYPE(current_mm) = token.id;
			
			step = 16;
			++i;
		}
		WHEN(16) {
			if (EXPECTED_METHOD_NAME(current_mm) == EXPECTED_CLASS_NAME(cm)) {
				CONSTRUCTOR_MATCH(cm) = current_mm;
			} else if (is_static) {
				++NUM_STATIC_METHODS(cm);
			} else {
				++NUM_METHODS(cm);
			}
			return nullptr;
		}
		WHEN(17) {
			MATCH(token.kind) {
			WHEN(TT::EXTENDS) {
				step = 18;
				++i;
			}
			OTHERWISE {
				SUPER_CLASS_NAME(cm) = "Object";
				step = 22;
			}
			}
		}
		WHEN_EXPECT(18, token.kind == TT::ID) {
			SUPER_CLASS_NAME(cm) = token.id;
			step = 22;
			++i;
		}
		WHEN_EXPECT(19, token.kind == TT::ID) {
			// .... var name token.id
			++i;
			step = 23;
		}
		WHEN(22) {
			return nullptr;
		}
		WHEN_EXPECT(23, token.kind == TT::COLON) {
			++i;
			step = 24;
		}
		WHEN_EXPECT(24, token.kind == TT::ID) {
			std::string token_id = token.id;
			if (token_id == "*")
				token_id.clear();
			if (is_static) {
				STATIC_VAR_TYPES(cm).emplace_back(token_id);
			} else {
				VAR_TYPES(cm).emplace_back(token_id);
			}
			++i;
			step = 25;
		}
		WHEN(25) {
			return nullptr;
		}
		OTHERWISE {
			return "reached error state";
		}
		}
	}
	return nullptr;
}

#define DIV "----------------------"

int main(int argc, char **argv)
{
	std::string line;
	int error_count = 0;
	int line_count = 0;
	while (std::getline(std::cin, line)) {
		++line_count;
		TokenVec tokens;
		std::istringstream ss(line);
		std::string chunk;
		ss >> chunk;
		Token token = parse_chunk(chunk);
		switch (token.kind)
		{
			case TT::PUBLIC:
			case TT::PRIVATE:
			case TT::INTERNAL:
			case TT::STATIC:
			case TT::VAR:
			case TT::CONST:
			case TT::CLASS:
			case TT::FUNCTION:
			case TT::VIRTUAL:
			case TT::OVERRIDE:
			case TT::GET:
			case TT::SET:
				break;
			default:
				continue;
		}
		tokens.emplace_back(token);
		while(ss >> chunk) {
			multi_parse_chunk(chunk, tokens);
		}
		tokens.emplace_back(TT::END);
		const char *const res = evaluate(tokens);
		if (res) {
			++error_count;
			std::cout << COLOR_RED DIV << std::endl;
			std::cout << "Parse error: " << res << std::endl;
			std::cout << "line " << line_count << ": " COLOR_YELLOW << line << std::endl;
			std::cout << COLOR_RED DIV COLOR_RESET << std::endl;
		}
	}
	if (error_count > 0) {
		std::cout << COLOR_RED DIV << std::endl;
		std::cout << error_count << " error(s) were encountered" << std::endl;
		std::cout << DIV COLOR_RESET << std::endl;
	}
}
