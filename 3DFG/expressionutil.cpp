#include "expressionutil.hpp"

namespace ExpressionUtil {

	// Peek next token, bounds check
	template<typename T>
	typename Expression<T>::Token Expression<T>::peek() {
		Token t = Token::None;
		if (indexTok < tokens.size()) {
			t = tokens[indexTok];
		}
		return t;
	}

	// [v]alue = literal | (e)
	template <typename T>
	T Expression<T>::parseValue() {
		T expression = 0;
		if (peek() == Token::Literal) {
			expression = literals[indexLit++];
			++indexTok;
		} else if (peek() == Token::OpenP) {
			// Consume
			++indexTok;
			try {
				expression = parseExpression();
			} catch (std::exception& e) {
				std::cout << e.what() << "\n";
			}
			// Mandate syntax
			if (peek() != Token::ClosedP) {
				throw std::runtime_error("ERROR::EXPRESSIONUTIL: Open parenthesis has no matching closed parenthesis.");;
			}
			// Consume RParens
			++indexTok;
		}
		return expression;
	}
	// [p]ower = function(v) | variable | v
	template <typename T>
	T Expression<T>::parsePower() {
		T value = 0;
		// Check sign
		int sign = 1;
		if (peek() == Token::Minus) {
			// Consume and change sign
			++indexTok;
			sign = -1;
		}
		// If the next token is a string
		if (peek() == Token::String) {
			++indexTok;
			// Check if it is a variable or a function name
			if (peek() == Token::OpenP) {
				// This is a function, execute assiciated function
				value = funcMap[strings[indexStr++]](parseValue());
			} else {
				// This is a variable, if there actually are variables
				if (!variables.empty()) {
					typename std::unordered_map<std::string, T>::iterator iter = variables.find(strings[indexStr]);
					if (iter != variables.end()) {
						// Found, get associated value
						value = iter->second;
						++indexStr;
					} else {
						// Map has variables but this one wasn't provided
						throw std::runtime_error("ERROR::EXPRESSIONUTIL: A variable wasn't initialized.");
					}
				} else {
					// Variable map is empty
					throw std::runtime_error("ERROR::EXPRESSIONUTIL: No variables provided or function is missing parenthesis.");
				}
			}
		} else {
			// Otherwise get the factor without any function
			try {
				value = parseValue();
			} catch (std::exception& e) {
				std::cout << e.what() << "\n";
			}
		}
		return sign * value;
	}
	// [f]actor = f ^ p | p
	template <typename T>
	T Expression<T>::parseFactor() {
		T power = parsePower();
		// Check if this is an exponent
		if (peek() == Token::Pow) {
			// Build a stack of powers in case there is more than one
			// This acounts for the intuition that 2 ^ 2 ^ 2 ^ 2 = 2 ^ (2 ^ (2 ^ 2))
			powerStack.clear();
			powerStack.push_back(power);
			while (peek() == Token::Pow) {
				// Consume and add to stack
				++indexTok;
				try {
					powerStack.push_back(parsePower());
				} catch (std::exception& e) {
					std::cout << e.what() << "\n";
				}
			}
			// Evaluate first two normally
			power = std::pow(powerStack[powerStack.size() - 2], powerStack[powerStack.size() - 1]);
			// Evaluate the rest moving backwards
			for (auto iter = powerStack.end() - 2; iter != powerStack.begin(); --iter) {
				power = std::pow(*iter, power);
			}
		}
		return power;
	}
	// [t]erm = t * f | t / f | f
	template <typename T>
	T Expression<T>::parseTerm() {
		T factor = parseFactor();
		while (peek() == Token::Multiply || peek() == Token::Divide) {
			if (peek() == Token::Multiply) {
				// Consume and multiply
				++indexTok;
				try {
					factor = factor * parseFactor();
				} catch (std::exception& e) {
					std::cout << e.what() << "\n";
				}
			} else if (peek() == Token::Divide) {
				// Consume and divide
				++indexTok;
				try {
					factor = factor / parseFactor();
				} catch (std::exception& e) {
					std::cout << e.what() << "\n";
				}
			}
		}
		return factor;
	}
	// [e]xpression = t + e | t - e | t
	template <typename T>
	T Expression<T>::parseExpression() {
		T term = parseTerm();
		while (peek() == Token::Plus || peek() == Token::Minus) {
			if (peek() == Token::Plus) {
				// Consume and add
				++indexTok;
				try {
					term = term + parseTerm();
				} catch (std::exception& e) {
					std::cout << e.what() << "\n";
				}
			} else if (peek() == Token::Minus) {
				// Consume and subtract
				++indexTok;
				try {
					term = term - parseTerm();
				} catch (std::exception& e) {
					std::cout << e.what() << "\n";
				}
			}
		}
		return term;
	}

	// Tokenize input string
	template <typename T>
	void Expression<T>::tokenize(std::string input) {
		// Set up temp values
		char currChar;
		T literalResult = 0;
		T fractional = 0;
		int length = 0;
		std::string stringResult = "";
		// Tokenize
		for (size_t i = 0; i < input.size(); i++) {
			// For each character
			switch (input[i]) {
				// Plus
			case '+':
				tokens.push_back(Token::Plus);
				break;
				// Minus
			case '-':
				tokens.push_back(Token::Minus);
				break;
				// Multiply
			case '*':
				tokens.push_back(Token::Multiply);
				break;
				// Divide
			case '/':
				tokens.push_back(Token::Divide);
				break;
				// Power
			case '^':
				tokens.push_back(Token::Pow);
				break;
				// Open parenthesis
			case '(':
				tokens.push_back(Token::OpenP);
				break;
				// Closed parenthesis
			case ')':
				tokens.push_back(Token::ClosedP);
				break;
			default:
				// If its a letter
				if (isalpha(input[i])) {
					stringResult = "";
					while (isalpha(input[i])) {
						currChar = input[i];
						i++;
						stringResult += currChar;
					}
					i--;
					tokens.push_back(Token::String);
					strings.push_back(stringResult);
					break;
				}
				// If its a number
				else if (isdigit(input[i])) {
					literalResult = 0;
					length = 0;
					fractional = 0;
					while (isdigit(input[i])) {
						currChar = input[i];
						i++;
						literalResult = literalResult * 10 + (currChar - '0');
					}
					// Check for a decimal
					if (input[i] == '.') {
						currChar = input[i];
						i++;
						// Error checking for non-digits or stray decimal separator
						if (!isdigit(input[i])) {
							throw std::runtime_error("ERROR::EXPRESSIONUTIL: Value following decimal separator is not a digit.");;
						}
						// Get the value after the decimal separator
						while (isdigit(input[i])) {
							currChar = input[i];
							i++;
							fractional = fractional * 10 + (currChar - '0');
							++length;
						}
					} else if (isalpha(input[i])) {
						throw std::runtime_error("ERROR::EXPRESSIONUTIL: Value following number is a letter. Multiplication must be explicit.");
					}
					if (fractional > 0) {
						literalResult = literalResult + (fractional / std::pow(10, length));
					}
					i--;
					tokens.push_back(Token::Literal);
					literals.push_back(literalResult);
					break;
				}
				break;
			}
		}
	}

	// Set the expression
	template <typename T>
	void Expression<T>::set(std::string expression) {
		// Clear
		strings.clear();
		tokens.clear();
		literals.clear();
		// Get string
		expressionString = expression;
		// Remove all white space
		expressionString.erase(remove_if(expressionString.begin(), expressionString.end(), isspace), expressionString.end());
		// Make lowercase
		std::transform(expressionString.begin(), expressionString.end(), expressionString.begin(), ::tolower);
		// Tokenize
		try {
			tokenize(expressionString);
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
	}

	// Solve expression
	template <typename T>
	T Expression<T>::solve() {
		// Initialize all indices to zero
		indexLit = 0;
		indexStr = 0;
		indexTok = 0;
		// Begin parsing
		T result = 0;
		try {
			result = parseExpression();
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
		return result;
	};

	// Check if the function is valid
	template <typename T>
	bool Expression<T>::isValid() {
		indexLit = 0;
		indexStr = 0;
		indexTok = 0;
		// Begin parsing
		T result = 0;
		try {
			result = parseExpression();
		} catch (std::exception& e) {
			std::cout << e.what() << "\n";
			return false;
		}
		return true;;
	}

	// Constructor
	template <typename T>
	Expression<T>::Expression() {}

	// Constructor that sets expression
	// e.g. ExpressionUtil::ExpressionFloat expr("x^2/sin(2*pi/y))-x/2");
	template <typename T>
	Expression<T>::Expression(std::string expression) {
		set(expression);
	}

	// Instantiation
	template class Expression<float>;
	template class Expression<double>;
}