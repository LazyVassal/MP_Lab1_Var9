#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <cctype>
#include <locale>

// Проверка наличия хотя бы одного символа из набора
bool contains_any_of(const std::string& s, const std::string& chars) {
    return s.find_first_of(chars) != std::string::npos;
}

// Генерация пароля
std::string generate_password(int length,
    bool use_upper, bool use_lower,
    bool use_digits, bool use_special) {
    const std::string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string lower = "abcdefghijklmnopqrstuvwxyz";
    const std::string digits = "0123456789";
    const std::string special = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    std::string chars;
    if (use_upper) chars += upper;
    if (use_lower) chars += lower;
    if (use_digits) chars += digits;
    if (use_special) chars += special;

    if (chars.empty()) {
        throw std::runtime_error("Должен быть выбран хотя бы один тип символов.");
    }

    // Современный генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);

    std::string password;
    password.reserve(length);
    for (int i = 0; i < length; ++i) {
        password += chars[dis(gen)];
    }
    return password;
}

// Оценка сложности пароля
std::string evaluate_strength(const std::string& password) {
    int score = 0;
    int len = password.length();

    // Длина
    if (len >= 12) score += 2;
    else if (len >= 8) score += 1;

    // Разнообразие символов
    bool has_upper = contains_any_of(password, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    bool has_lower = contains_any_of(password, "abcdefghijklmnopqrstuvwxyz");
    bool has_digit = contains_any_of(password, "0123456789");
    bool has_special = contains_any_of(password, "!@#$%^&*()-_=+[]{}|;:,.<>?");

    int types_count = has_upper + has_lower + has_digit + has_special;
    if (types_count >= 3) score += 2;
    else if (types_count == 2) score += 1;

    // Простые последовательности (3 и более подряд)
    for (int i = 0; i <= len - 3; ++i) {
        char a = password[i], b = password[i + 1], c = password[i + 2];
        // abc, bcd, ... xyz (регистр не важен)
        if (std::isalpha(a) && std::isalpha(b) && std::isalpha(c)) {
            char a_l = std::tolower(a), b_l = std::tolower(b), c_l = std::tolower(c);
            if ((b_l == a_l + 1 && c_l == b_l + 1)) {
                score -= 2;
                break;
            }
        }
        // 123, 234, ... 890
        if (std::isdigit(a) && std::isdigit(b) && std::isdigit(c)) {
            if ((b == a + 1 && c == b + 1)) {
                score -= 2;
                break;
            }
        }
        // qwerty, asdf, zxcv (регистр не важен)
        static const char* keyboard[] = { "qwerty", "asdf", "zxcv" };
        for (const char* seq : keyboard) {
            int seq_len = strlen(seq);
            for (int j = 0; j <= seq_len - 3; ++j) {
                bool found = true;
                for (int k = 0; k < 3; ++k) {
                    if (std::tolower(password[i + k]) != seq[j + k]) {
                        found = false;
                        break;
                    }
                }
                if (found) { score -= 2; break; }
            }
            if (score < 0) break;
        }
        if (score < 0) break;
    }

    // Словарные слова
    static const char* common_words[] = { "password", "admin", "user", "test", "qwerty", "123456" };
    for (const char* word : common_words) {
        if (password.find(word) != std::string::npos ||
            password.find(std::string(word).c_str()) != std::string::npos ||
            password.find(std::string(word).c_str()) != std::string::npos) {
            score -= 3;
            break;
        }
        // Поиск без учёта регистра
        std::string p_lower = password;
        std::transform(p_lower.begin(), p_lower.end(), p_lower.begin(),
            [](unsigned char c) { return std::tolower(c); });
        if (p_lower.find(word) != std::string::npos) {
            score -= 3;
            break;
        }
    }

    // Итоговая оценка
    if (score <= 0) return "Очень слабый";
    else if (score <= 3) return "Слабый";
    else if (score <= 6) return "Средний";
    else if (score <= 8) return "Сильный";
    else return "Очень сильный";
}

int main() {
    std::locale::global(std::locale("Russian_Russia.1251"));
    try {
        int length;
        bool use_upper, use_lower, use_digits, use_special;

        std::cout << "Введите длину пароля: ";
        std::cin >> length;

        char ans;
        std::cout << "Включать заглавные буквы? (y/n): ";
        std::cin >> ans; use_upper = (ans == 'y' || ans == 'Y');

        std::cout << "Включать строчные буквы? (y/n): ";
        std::cin >> ans; use_lower = (ans == 'y' || ans == 'Y');

        std::cout << "Включать цифры? (y/n): ";
        std::cin >> ans; use_digits = (ans == 'y' || ans == 'Y');

        std::cout << "Включать специальные символы? (y/n): ";
        std::cin >> ans; use_special = (ans == 'y' || ans == 'Y');

        std::string password = generate_password(length, use_upper, use_lower, use_digits, use_special);
        std::string strength = evaluate_strength(password);

        std::cout << "\nСгенерированный пароль: " << password << "\n";
        std::cout << "Оценка сложности: " << strength << "\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}