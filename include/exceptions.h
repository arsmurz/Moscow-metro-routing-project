#pragma once

#include <stdexcept>
#include <string>

// базовое исключение
class MetroException : public std::runtime_error {
public:
  explicit MetroException(const std::string &msg) : std::runtime_error(msg) {}
};

// ошибки графа
class GraphException : public MetroException {
public:
  explicit GraphException(const std::string &msg) : MetroException(msg) {}
};

class StationNotFoundException : public GraphException {
public:
  explicit StationNotFoundException(size_t id)
      : GraphException("Станция с ID " + std::to_string(id) + " не найдена") {}

  explicit StationNotFoundException(const std::string &name)
      : GraphException("Станция '" + name + "' не найдена") {}
};

class DuplicateStationException : public GraphException {
public:
  explicit DuplicateStationException(const std::string &name)
      : GraphException("Станция '" + name + "' уже существует") {}
};

class EdgeNotFoundException : public GraphException {
public:
  EdgeNotFoundException(size_t from, size_t to)
      : GraphException("Ребро " + std::to_string(from) + " -> " +
                       std::to_string(to) + " не найдено") {}
};

class InvalidIdException : public GraphException {
public:
  explicit InvalidIdException(size_t id)
      : GraphException("Некорректный ID: " + std::to_string(id)) {}
};

// ошибки сериализации
class SerializationException : public MetroException {
public:
  explicit SerializationException(const std::string &msg)
      : MetroException(msg) {}
};

class FileNotFoundException : public SerializationException {
public:
  explicit FileNotFoundException(const std::string &filename)
      : SerializationException("Файл не найден: " + filename) {}
};

class InvalidFormatException : public SerializationException {
public:
  explicit InvalidFormatException(const std::string &filename)
      : SerializationException("Неверный формат файла: " + filename) {}
};

class UnsupportedVersionException : public SerializationException {
public:
  UnsupportedVersionException(int version, int expected)
      : SerializationException("Неподдерживаемая версия " +
                               std::to_string(version) + ". Ожидается " +
                               std::to_string(expected)) {}
};

class JsonParseException : public SerializationException {
public:
  explicit JsonParseException(const std::string &details)
      : SerializationException("Ошибка парсинга JSON: " + details) {}
};

// ошибки поиска
class RoutingException : public MetroException {
public:
  explicit RoutingException(const std::string &msg) : MetroException(msg) {}
};

class NoPathFoundException : public RoutingException {
public:
  NoPathFoundException(const std::string &from, const std::string &to)
      : RoutingException("Маршрут от '" + from + "' до '" + to +
                         "' не найден") {}
};