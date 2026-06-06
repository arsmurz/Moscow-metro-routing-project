#pragma once

#include "graph.h"
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class ISerializationStrategy {
public:
  virtual ~ISerializationStrategy() = default;

  virtual bool save(const FastModificationGraph &graph,
                    const std::string &filename) = 0;
  virtual FastModificationGraph load(const std::string &filename) = 0;
  virtual std::string getExtension() const = 0;
};

// Паттерн стратегии
class BinaryStrategy : public ISerializationStrategy {
public:
  bool save(const FastModificationGraph &graph,
            const std::string &filename) override;
  FastModificationGraph load(const std::string &filename) override;
  std::string getExtension() const override { return "bin"; }

private:
  void writeStations(std::ofstream &file, const FastModificationGraph &graph);
  void writeEdges(std::ofstream &file, const FastModificationGraph &graph);
  void readStations(std::ifstream &file, FastModificationGraph &graph);
  void readEdges(std::ifstream &file, FastModificationGraph &graph);
};

class JSONStrategy : public ISerializationStrategy {
public:
  bool save(const FastModificationGraph &graph,
            const std::string &filename) override;
  FastModificationGraph load(const std::string &filename) override;
  std::string getExtension() const override { return "json"; }
};

class TextStrategy : public ISerializationStrategy {
public:
  bool save(const FastModificationGraph &graph,
            const std::string &filename) override;
  FastModificationGraph load(const std::string &filename) override;
  std::string getExtension() const override { return "txt"; }
};

// сериализатор - разделение ответственности
class GraphSerializer {
public:
  GraphSerializer();

  void setFormat(const std::string &format);
  void setFormatByExtension(const std::string &filename);
  bool save(const FastModificationGraph &graph, const std::string &filename);
  FastModificationGraph load(const std::string &filename);
  std::vector<std::string> getAvailableFormats() const;

private:
  std::unique_ptr<ISerializationStrategy> strategy;
  std::unique_ptr<ISerializationStrategy>
  createStrategy(const std::string &format);
};