import random
import string
import re

def get_user_settings():
    length = int(input("Введите длину пароля: "))
    use_upper = input("Включать заглавные буквы? (y/n): ").lower() == 'y'
    use_lower = input("Включать строчные буквы? (y/n): ").lower() == 'y'
    use_digits = input("Включать цифры? (y/n): ").lower() == 'y'
    use_special = input("Включать специальные символы? (y/n): ").lower() == 'y'
    return length, use_upper, use_lower, use_digits, use_special

def generate_password(length, use_upper, use_lower, use_digits, use_special):
    chars = ''
    if use_upper: chars += string.ascii_uppercase
    if use_lower: chars += string.ascii_lowercase
    if use_digits: chars += string.digits
    if use_special: chars += string.punctuation

    if not chars:
        raise ValueError("Должен быть выбран хотя бы один тип символов.")

    password = ''.join(random.choices(chars, k=length))
    return password

def evaluate_strength(password):
    score = 0
    length = len(password)

    # Длина
    if length >= 12: score += 2
    elif length >= 8: score += 1

    # Разнообразие символов
    has_upper = bool(re.search(r'[A-Z]', password))
    has_lower = bool(re.search(r'[a-z]', password))
    has_digit = bool(re.search(r'\d', password))
    has_special = bool(re.search(r'[!@#$%^&*(),.?":{}|<>]', password))

    types_count = sum([has_upper, has_lower, has_digit, has_special])
    if types_count >= 3: score += 2
    elif types_count == 2: score += 1

    # Простые последовательности (3 и более подряд)
    if re.search(r'(abc|bcd|cde|def|efg|fgh|ghi|hij|ijk|jkl|klm|lmn|mno|nop|opq|pqr|qrs|rst|stu|tuv|uvw|vwx|wxy|xyz)', password.lower()):
        score -= 2
    if re.search(r'(123|234|345|456|567|678|789|890)', password):
        score -= 2
    if re.search(r'(qwerty|asdf|zxcv)', password.lower()):
        score -= 2

    # Словарные слова (базовая проверка)
    common_words = ['password', 'admin', 'user', 'test', 'qwerty', '123456']
    for word in common_words:
        if word in password.lower():
            score -= 3

    # Итоговая оценка
    if score <= 0: return "Очень слабый"
    elif score <= 3: return "Слабый"
    elif score <= 6: return "Средний"
    elif score <= 8: return "Сильный"
    else: return "Очень сильный"

def main():
    try:
        settings = get_user_settings()
        password = generate_password(*settings)
        strength = evaluate_strength(password)
        print("\nСгенерированный пароль:", password)
        print("Оценка сложности:", strength)
    except ValueError as e:
        print("Ошибка:", e)

if __name__ == "__main__":
    main()