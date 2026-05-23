#pragma once
#include "../include/routing.h"

struct CostWeight{
  private:
    double weight;
  public:
    bool operator < (CostWeight right) {
        return weight < right.weight;
    }
    bool operator > (CostWeight right) {
        return weight > right.weight;
    }
    bool operator <= (CostWeight right) {
        return weight <= right.weight;
    }
    bool operator >= (CostWeight right) {
        return weight >= right.weight;
    }
    CostWeight operator += (CostWeight right) {
        weight += right.weight;
    }
    CostWeight operator -= (CostWeight right) {
        weight -= right.weight;
    }
    CostWeight(Direction dir) {
        weight = dir.weight;
    }
};

struct TimeWeight{
  private:
    double time;
  public:
    bool operator < (TimeWeight right) {
        return time < right.time;
    }
    bool operator > (TimeWeight right) {
        return time > right.time;
    }
    bool operator <= (TimeWeight right) {
        return time <= right.time;
    }
    bool operator >= (TimeWeight right) {
        return time >= right.time;
    }
    CostWeight operator += (TimeWeight right) {
        time += right.time;
    }
    CostWeight(Direction dir) {
        time = dir.time;
    }
};

struct TransferWeight{
  private:
    int transfer;
  public:
    bool operator < (TransferWeight right) {
        return transfer < right.transfer;
    }
    bool operator > (TransferWeight right) {
        return transfer > right.transfer;
    }
    bool operator <= (TransferWeight right) {
        return transfer <= right.transfer;
    }
    bool operator >= (TransferWeight right) {
        return transfer >= right.transfer;
    }
    TransferWeight operator += (TransferWeight right) {
        transfer += right.transfer;
    }
    CostWeight(Direction dir) {
        transfer = dir.istransfer;
    }
};

