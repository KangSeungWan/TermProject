#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <math.h>
#include <stdexcept>
#include <cctype>
#include <safeint.h>

class calculator {
private:
	static calculator* instance; // calculator 객체

	std::vector<char> postfix; // 후위식으로 변환한 값 저장
	std::string input; // 들어온 입력
	int ctn = 0; // 계산 중 생략하는 횟수

	void init() { // 벡터 초기화, 예외처리 함수 호출
		postfix.clear();

		checkError();
	}

	void checkError() { // 예외 찾아서 throw

		// 문자가 있는 경우 (0x, 0b 제외)

		if (!isdigit(input.at(0)) && input.at(0) != '(' && input.at(0) != '{' && input.at(0) != '[') throw std::runtime_error("[오류] 숫자가 아닌 문자가 입력되었습니다.\n"); // 첫 자리 숫자 아닌 경우 에러

		for (int i = 1; i < input.size(); i++) {
			if (!isdigit(input.at(i)) && !isoper(input.at(i)) && !isBracket(input.at(i))) { // 숫자가 아니면서 연산자도 아닌 경우, 괄호도 아닌 경우

				if (input.at(i) != 'b' && input.at(i) != 'x') throw std::runtime_error("[오류] 숫자가 아닌 문자가 입력되었습니다.\n");

				if (input.at(i - 1) != '0') throw std::runtime_error("[오류] 숫자가 아닌 문자가 입력되었습니다.\n");

				if ((i + 1 >= input.size()) || !isdigit(input.at(i + 1))) throw std::runtime_error("[오류] 숫자가 아닌 문자가 입력되었습니다.\n");
			}
		}


		// 연산자가 연속으로 오는 경우
		int op_flag = 0;
		for (char c : input) {

			if (isoper(c) && c != '(' && c != ')') {
				if (op_flag == 0) {
					op_flag = 1;
					continue;
				}
				throw std::runtime_error("[오류] 연산자가 연속으로 입력되었습니다.\n");
			}
			else {
				op_flag = 0;
			}

		}



		// 괄호 쌍 맞지 않는 경우 (스택활용 처리)
		std::stack<char> stack;
		bool only_Bracket = 1;

		for (char c : input) {

			switch (isBracket(c)) {
			case 1: // 여는 괄호
				stack.push(c);
				break;
			case 2: // 닫는 괄호
				if (stack.empty()) throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");

				if (c == ')') {
					if (stack.top() == '(') stack.pop();

				}
				else if (c == '}') {
					if (stack.top() == '{') stack.pop();

				}
				else { // c == ']'
					if (stack.top() == '[') stack.pop();

				}
				break;
			default: // 괄호 아님
				if (isdigit(c)) only_Bracket = 0;
				break;
			}

		}

		if (!stack.empty()) throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");

		if (only_Bracket) throw std::runtime_error("[오류] 존재하지 않는 계산형식 입니다.\n");

	}

	int isBracket(char c) {
		if (c == '(' || c == '{' || c == '[') return 1; // 여는 괄호
		if (c == ')' || c == '}' || c == ']') return 2; // 닫는 괄호
		return 0; // 괄호 아님
	}

	bool isoper(char c) {
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') return 1;
		return 0;
	}

	int getPriority(char op) { // 우선순위
		switch (op) {
		case '*':
		case '/':
			return 2;
		case '+':
		case '-':
			return 1;
		case '(':
		case '{':
		case '[':
			return 0;
		case ')':
		case '}':
		case ']':
			return -1;
		default:
			return -2;
		}
	}

	bool isDecimal(std::string s, int idx) {
		if (s[idx] == '0') {
			if (idx + 1 >= s.size()) {
				return 1;
			}
			if (s[idx + 1] == 'x' || s[idx + 1] == 'b') {
				return 0;
			}
			return 1;
		}
		return 1;
	}

	int convert(std::string s, int idx) { // 진수 변환
		int range = 2;
		while (idx + range < s.size() && getPriority(s[idx + range]) == -2) {
			range++;
		}
		range--;
		ctn = range;

		int shiftValue = (s[idx + 1] == 'x') ? 4 : 1;
		int result = 0;
		for (int shfitTime = 0; range >= 2; shfitTime++) {
			result += (s[idx + range] - '0') * (1 << (shfitTime * shiftValue));
			range--;
		}

		return result;
	}

	int charToInt(std::vector<char> vec, int idx) {

		int range = 1;
		while (idx + range < vec.size() && vec[idx + range] != '?') {

			//std::cout << "vec[idx + range] :  " << vec[idx + range] << std::endl;

			range++;
		}
		range--;
		ctn = range;

		//std::cout << "range :  " << range << std::endl;

		msl::utilities::SafeInt<int> result = 0; // 오버플로우 방지
		for (int i = 0; range >= 0; i++) {

			int tmp = vec[idx + range] - '0';

			if (vec[idx + range] == '?' || getPriority(vec[idx + range]) != -2) {
				break;
			}

			//std::cout << "문자 " << vec[idx+range] << std::endl;
			//std::cout << "곱하기 전 tmp : " << tmp << std::endl;

			for (int j = 0; j < i; j++) {
				tmp *= 10;
			}

			//std::cout << "곱한 후 tmp : " << tmp << std::endl;

			result += tmp;

			//std::cout << "result += tmp : " << result << std::endl;


			range--;
		}

		// std::cout << "value " << result << std::endl;

		return result;
	}

	int charNum(std::string str, int idx) { // char로 이루어진 숫자의 범위
		int range = 1;
		while (getPriority(str[idx + range]) == -2 && idx + range < str.size()) {
			range++;
		}
		range--;
		return range;
	}

	void infixToPostfix() { // 중위식 -> 후위식

		std::stack<char> stack;
		int idx = 0;

		for (char c : input) {

			if (ctn) { // 2진수, 16진수일 경우 뒷 자리 무시
				ctn--;
				idx++;

				if (ctn == 0) {
					postfix.push_back('?'); // 수를 구분짓기 위해 ? 삽입
				}

				continue;
			}

			int priority = getPriority(c);
			int stackPriority = 0;
			int limit = 0;
			int decimalValue = 0;

			switch (priority) {
			case -2: // 숫자
				if (isDecimal(input, idx)) { // 10진수 처리
					postfix.push_back(c);
					if (charNum(input, idx) == 0) {
						postfix.push_back('?'); // 수를 구분짓기 위해 ? 삽입
					}
				}
				else {  // 2진수, 16진수 처리
					decimalValue = (convert(input, idx)); // 10진수로 만들고 char로 변환해서 push
					char charValue[100];
					sprintf(charValue, "%d", decimalValue);
					limit = (int)log10(decimalValue + 1);
					for (int i = 0; i <= limit; i++) {
						postfix.push_back(charValue[i]);
					}
				}

				break;
			case 2: // 곱셈, 나눗셈
			case 1: // 덧셈, 뺄셈
				if (stack.empty()) { // 스택이 비어있는 경우
					stack.push(c);
					break;
				}

				stackPriority = getPriority(stack.top());
				if (priority > stackPriority) {
					stack.push(c);
				}
				else {
					postfix.push_back(stack.top());
					stack.pop();
					stack.push(c);
				}
				break;
			case 0: // 여는 괄호
				stack.push(c);
				break;
			case -1: // 닫는 괄호
				if (stack.empty()) throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");

				while (getPriority(stack.top())) {
					postfix.push_back(stack.top());
					stack.pop();
					if (stack.empty()) throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");
				}

				switch (c) {
				case ')':
					if (stack.top() != '(') throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");
					break;
				case '}':
					if (stack.top() != '{') throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");
					break;
				case ']':
					if (stack.top() != '[') throw std::runtime_error("[오류] 괄호쌍이 맞지 않습니다.\n");
					break;
				default:
					break;
				}

				stack.pop();
				break;
			}
			idx++;
		}

		while (!stack.empty()) { // 스택에 남은 연산자 빼내기
			postfix.push_back(stack.top());
			stack.pop();
		}
	}

	int calculate(int num1, int num2, char oper) {
		msl::utilities::SafeInt<int> result = 0; // 오버플로우 방지

		switch (oper) {
		case '+':
			result = (msl::utilities::SafeInt<int>)num1 + (msl::utilities::SafeInt<int>)num2;
			break;
		case '-':
			result = (msl::utilities::SafeInt<int>)num1 - (msl::utilities::SafeInt<int>)num2;
			break;
		case '*':
			result = (msl::utilities::SafeInt<int>)num1 * (msl::utilities::SafeInt<int>)num2;
			break;
		case '/':
			if (num2 == 0) throw std::runtime_error("[오류] 0으로 나누기를 시도하였습니다.\n");
			result = (msl::utilities::SafeInt<int>)num1 / (msl::utilities::SafeInt<int>)num2;
			break;
		default:
			throw std::runtime_error("[오류] 올바른 연산자가 아닙니다.\n");
		}

		return result;
	}

	void setString(std::string s) {
		this->input = s;
	}

	calculator(std::string input) { // 생성자
		this->input = input;
	}

public:
	int getResult() { // 후위식 계산

		init();

		infixToPostfix();

		//for (int i = 0; i < postfix.size(); i++) { // 예시 출력
		//	std::cout << postfix[i];
		//}
		//std::cout << "\n";

		std::stack<int> stack;
		int idx = 0;

		for (char c : postfix) {

			//std::cout << c << std::endl;


			if (ctn) {
				ctn--;
				idx++;
				continue;
			}

			int p = getPriority(c);
			int num2, num1;

			switch (p) {
			case -2: // 숫자
				if (c == '?') break;
				stack.push(charToInt(postfix, idx));

				break;
			case 1:	// 덧셈,뺄셈
			case 2: // 곱셈, 나눗셈

				if (stack.empty()) throw std::runtime_error("[오류] 옳지 않은 수식입니다.\n");
				num2 = stack.top();
				stack.pop();
				if (stack.empty()) throw std::runtime_error("[오류] 옳지 않은 수식입니다.\n");
				num1 = stack.top();
				stack.pop();
				stack.push(calculate(num1, num2, c));

				//std::cout << "claculate and push : " << calculate(num1, num2, c) << std::endl;

				break;
			default:
				break;
			}
			idx++;
		}

		int result = stack.top();
		stack.pop();

		return result;
	}

	static calculator* getInstance(std::string s) {
		if (instance == 0) {
			instance = new calculator(s);
		}
		else {
			instance->setString(s);
		}
		return instance;
	}

	static void resetInstance() {
		if (instance != NULL) {
			delete instance;
		}
		instance = NULL;
	}

};

calculator* calculator::instance = 0;

class Factory {
public:
	virtual void makeAndPlay() = 0;
};

class CalculatorFactory : public Factory {
public:
	void makeAndPlay() override {
		std::string string_input;

		std::cout << "------------- 계산기 프로그램을 시작합니다 -------------\n" << std::endl;


		while (1) {

			std::cout << "[입력] 원하는 계산을 한 줄로 입력해주세요(엔터 입력시 계산을 수행합니다) : ";
			std::getline(std::cin, string_input);

			string_input.erase(std::remove(string_input.begin(), string_input.end(), ' '), string_input.end()); // 공백 제거

			if (!string_input.compare("exit") || !string_input.compare("Exit")) { // 종료명령
				std::cout << "\n------------- 계산기 프로그램을 종료합니다 -------------" << std::endl;
				break;
			}


			if (!string_input.compare("")) { // 빈 문자열 입력
				std::cout << "\n[오류] 빈 문자열입니다.\n\n" << std::endl;

				continue;
			}

			calculator* cal = calculator::getInstance(string_input);  // singleton 패턴으로 객체 한번만 생성하도록.

			int result = 0;

			try { // 예외처리

				std::cout << "\n[대기] 계산중.........\n" << std::endl;
				result = cal->getResult(); // 계산 수행
				std::cout << "[출력] " << string_input << " 의 결과는 ( " << result << " ) 입니다. (exit 또는 Exit 입력 시 종료)\n\n" << std::endl;
			}
			catch (std::runtime_error& e) {
				std::cout << e.what() << "\n" << std::endl;
			}
			catch (msl::utilities::SafeIntException e) {
				std::cout << "[오류] 오버플로우가 발생했습니다.\n" << std::endl;
				std::cout << "[종료] 심각한 오류로 프로그램을 종료합니다.\n" << std::endl;
				std::cout << "------------- 계산기 프로그램을 종료합니다 -------------" << std::endl;
				break;
			}
			catch (...) {
				std::cout << "[오류] 예외가 발생했습니다.\n" << std::endl;
			}

		}

		calculator::resetInstance(); // delete
	}
};

int main() {

	Factory* myFactory = new CalculatorFactory();
	myFactory->makeAndPlay();
	delete myFactory;

}