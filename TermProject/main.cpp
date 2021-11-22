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
	static calculator* instance; // calculator ��ü

	std::vector<char> postfix; // ���������� ��ȯ�� �� ����
	std::string input; // ���� �Է�
	int ctn = 0; // ��� �� �����ϴ� Ƚ��

	void init() { // ���� �ʱ�ȭ, ����ó�� �Լ� ȣ��
		postfix.clear();

		checkError();
	}

	void checkError() { // ���� ã�Ƽ� throw

		// ���ڰ� �ִ� ��� (0x, 0b ����)

		if (!isdigit(input.at(0)) && input.at(0) != '(' && input.at(0) != '{' && input.at(0) != '[') throw std::runtime_error("[����] ���ڰ� �ƴ� ���ڰ� �ԷµǾ����ϴ�.\n"); // ù �ڸ� ���� �ƴ� ��� ����

		for (int i = 1; i < input.size(); i++) {
			if (!isdigit(input.at(i)) && !isoper(input.at(i)) && !isBracket(input.at(i))) { // ���ڰ� �ƴϸ鼭 �����ڵ� �ƴ� ���, ��ȣ�� �ƴ� ���

				if (input.at(i) != 'b' && input.at(i) != 'x') throw std::runtime_error("[����] ���ڰ� �ƴ� ���ڰ� �ԷµǾ����ϴ�.\n");

				if (input.at(i - 1) != '0') throw std::runtime_error("[����] ���ڰ� �ƴ� ���ڰ� �ԷµǾ����ϴ�.\n");

				if ((i + 1 >= input.size()) || !isdigit(input.at(i + 1))) throw std::runtime_error("[����] ���ڰ� �ƴ� ���ڰ� �ԷµǾ����ϴ�.\n");
			}
		}


		// �����ڰ� �������� ���� ���
		int op_flag = 0;
		for (char c : input) {

			if (isoper(c) && c != '(' && c != ')') {
				if (op_flag == 0) {
					op_flag = 1;
					continue;
				}
				throw std::runtime_error("[����] �����ڰ� �������� �ԷµǾ����ϴ�.\n");
			}
			else {
				op_flag = 0;
			}

		}



		// ��ȣ �� ���� �ʴ� ��� (����Ȱ�� ó��)
		std::stack<char> stack;
		bool only_Bracket = 1;

		for (char c : input) {

			switch (isBracket(c)) {
			case 1: // ���� ��ȣ
				stack.push(c);
				break;
			case 2: // �ݴ� ��ȣ
				if (stack.empty()) throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");

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
			default: // ��ȣ �ƴ�
				if (isdigit(c)) only_Bracket = 0;
				break;
			}

		}

		if (!stack.empty()) throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");

		if (only_Bracket) throw std::runtime_error("[����] �������� �ʴ� ������� �Դϴ�.\n");

	}

	int isBracket(char c) {
		if (c == '(' || c == '{' || c == '[') return 1; // ���� ��ȣ
		if (c == ')' || c == '}' || c == ']') return 2; // �ݴ� ��ȣ
		return 0; // ��ȣ �ƴ�
	}

	bool isoper(char c) {
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') return 1;
		return 0;
	}

	int getPriority(char op) { // �켱����
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

	int convert(std::string s, int idx) { // ���� ��ȯ
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

		msl::utilities::SafeInt<int> result = 0; // �����÷ο� ����
		for (int i = 0; range >= 0; i++) {

			int tmp = vec[idx + range] - '0';

			if (vec[idx + range] == '?' || getPriority(vec[idx + range]) != -2) {
				break;
			}

			//std::cout << "���� " << vec[idx+range] << std::endl;
			//std::cout << "���ϱ� �� tmp : " << tmp << std::endl;

			for (int j = 0; j < i; j++) {
				tmp *= 10;
			}

			//std::cout << "���� �� tmp : " << tmp << std::endl;

			result += tmp;

			//std::cout << "result += tmp : " << result << std::endl;


			range--;
		}

		// std::cout << "value " << result << std::endl;

		return result;
	}

	int charNum(std::string str, int idx) { // char�� �̷���� ������ ����
		int range = 1;
		while (getPriority(str[idx + range]) == -2 && idx + range < str.size()) {
			range++;
		}
		range--;
		return range;
	}

	void infixToPostfix() { // ������ -> ������

		std::stack<char> stack;
		int idx = 0;

		for (char c : input) {

			if (ctn) { // 2����, 16������ ��� �� �ڸ� ����
				ctn--;
				idx++;

				if (ctn == 0) {
					postfix.push_back('?'); // ���� �������� ���� ? ����
				}

				continue;
			}

			int priority = getPriority(c);
			int stackPriority = 0;
			int limit = 0;
			int decimalValue = 0;

			switch (priority) {
			case -2: // ����
				if (isDecimal(input, idx)) { // 10���� ó��
					postfix.push_back(c);
					if (charNum(input, idx) == 0) {
						postfix.push_back('?'); // ���� �������� ���� ? ����
					}
				}
				else {  // 2����, 16���� ó��
					decimalValue = (convert(input, idx)); // 10������ ����� char�� ��ȯ�ؼ� push
					char charValue[100];
					sprintf(charValue, "%d", decimalValue);
					limit = (int)log10(decimalValue + 1);
					for (int i = 0; i <= limit; i++) {
						postfix.push_back(charValue[i]);
					}
				}

				break;
			case 2: // ����, ������
			case 1: // ����, ����
				if (stack.empty()) { // ������ ����ִ� ���
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
			case 0: // ���� ��ȣ
				stack.push(c);
				break;
			case -1: // �ݴ� ��ȣ
				if (stack.empty()) throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");

				while (getPriority(stack.top())) {
					postfix.push_back(stack.top());
					stack.pop();
					if (stack.empty()) throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");
				}

				switch (c) {
				case ')':
					if (stack.top() != '(') throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");
					break;
				case '}':
					if (stack.top() != '{') throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");
					break;
				case ']':
					if (stack.top() != '[') throw std::runtime_error("[����] ��ȣ���� ���� �ʽ��ϴ�.\n");
					break;
				default:
					break;
				}

				stack.pop();
				break;
			}
			idx++;
		}

		while (!stack.empty()) { // ���ÿ� ���� ������ ������
			postfix.push_back(stack.top());
			stack.pop();
		}
	}

	int calculate(int num1, int num2, char oper) {
		msl::utilities::SafeInt<int> result = 0; // �����÷ο� ����

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
			if (num2 == 0) throw std::runtime_error("[����] 0���� �����⸦ �õ��Ͽ����ϴ�.\n");
			result = (msl::utilities::SafeInt<int>)num1 / (msl::utilities::SafeInt<int>)num2;
			break;
		default:
			throw std::runtime_error("[����] �ùٸ� �����ڰ� �ƴմϴ�.\n");
		}

		return result;
	}

	void setString(std::string s) {
		this->input = s;
	}

	calculator(std::string input) { // ������
		this->input = input;
	}

public:
	int getResult() { // ������ ���

		init();

		infixToPostfix();

		//for (int i = 0; i < postfix.size(); i++) { // ���� ���
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
			case -2: // ����
				if (c == '?') break;
				stack.push(charToInt(postfix, idx));

				break;
			case 1:	// ����,����
			case 2: // ����, ������

				if (stack.empty()) throw std::runtime_error("[����] ���� ���� �����Դϴ�.\n");
				num2 = stack.top();
				stack.pop();
				if (stack.empty()) throw std::runtime_error("[����] ���� ���� �����Դϴ�.\n");
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

		std::cout << "------------- ���� ���α׷��� �����մϴ� -------------\n" << std::endl;


		while (1) {

			std::cout << "[�Է�] ���ϴ� ����� �� �ٷ� �Է����ּ���(���� �Է½� ����� �����մϴ�) : ";
			std::getline(std::cin, string_input);

			string_input.erase(std::remove(string_input.begin(), string_input.end(), ' '), string_input.end()); // ���� ����

			if (!string_input.compare("exit") || !string_input.compare("Exit")) { // ������
				std::cout << "\n------------- ���� ���α׷��� �����մϴ� -------------" << std::endl;
				break;
			}


			if (!string_input.compare("")) { // �� ���ڿ� �Է�
				std::cout << "\n[����] �� ���ڿ��Դϴ�.\n\n" << std::endl;

				continue;
			}

			calculator* cal = calculator::getInstance(string_input);  // singleton �������� ��ü �ѹ��� �����ϵ���.

			int result = 0;

			try { // ����ó��

				std::cout << "\n[���] �����.........\n" << std::endl;
				result = cal->getResult(); // ��� ����
				std::cout << "[���] " << string_input << " �� ����� ( " << result << " ) �Դϴ�. (exit �Ǵ� Exit �Է� �� ����)\n\n" << std::endl;
			}
			catch (std::runtime_error& e) {
				std::cout << e.what() << "\n" << std::endl;
			}
			catch (msl::utilities::SafeIntException e) {
				std::cout << "[����] �����÷ο찡 �߻��߽��ϴ�.\n" << std::endl;
				std::cout << "[����] �ɰ��� ������ ���α׷��� �����մϴ�.\n" << std::endl;
				std::cout << "------------- ���� ���α׷��� �����մϴ� -------------" << std::endl;
				break;
			}
			catch (...) {
				std::cout << "[����] ���ܰ� �߻��߽��ϴ�.\n" << std::endl;
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