#ifndef EXPRUTIL_H
#define EXPRUTIL_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>

namespace ExprUtil {

	template <typename T>
	class Expression {
	private:

		// Holds possible tokens
		enum class Token {
			None,
			Plus,
			Minus,
			Multiply,
			Divide,
			Pow,
			OpenP,
			ClosedP,
			String,
			Literal
		};

		// Associate string with a function
		typedef T(*FnPtr)(T);
		std::unordered_map<std::string, FnPtr> funcMap{
			{"sin", sin},
			{"cos", cos},
			{"tan", tan},
			{"abs", abs},
			{"exp", exp},
			{"log", log},
			{"sqrt", sqrt}
		};

		// Processed expression string
		std::string expressionString;

		// Tokens
		std::vector<Token> tokens;
		int indexTok = 0;
		std::vector<T> literals;
		int indexLit = 0;
		std::vector<std::string> strings;
		int indexStr = 0;

		// Pi
		T const pi = std::acos(-T(1));;

		// Power stack 
		std::vector<T> powerStack;

		// Peek next token, bounds check
		typename Token peek();

		// [v]alue = literal | (e)
		T parseValue();
		// [p]ower = function(v) | variable | v
		T parsePower();
		// [f]actor = f ^ p | p
		T parseFactor();
		// [t]erm = t * f | t / f | f
		T parseTerm();
		// [e]xpression = t + e | t - e | t
		T parseExpression();

		// Tokenize input string
		void tokenize(std::string input);


	public:
		// Associate a variable with a value
		// e.g. myExpr.variables["x"] = 4;
		std::unordered_map<std::string, T> variables = { {"pi", pi} };
		// Set expression
		void set(std::string expression);
		// Solve expression
		T solve();
		// Check if the function is valid by solving once
		bool isValid();
		// Constructor
		Expression();
		// Constructor that sets expression
		// e.g. ExprUtil::ExpressionFloat expr("x^2/sin(2*pi/y)-x/2");
		Expression(std::string expression);
	};

	// Instantiation
	typedef Expression<float> ExprFloat;
	typedef Expression<double> ExprDouble;
}

#endif