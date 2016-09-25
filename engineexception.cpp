#include "engineexception.h"

EngineException::EngineException() : QException() { }

EngineException::EngineException(QString errorMsg) : QException() {
    this->errorMsg = errorMsg;
}

EngineException::EngineException(const EngineException &other) : QException() {
    this->errorMsg = other.errorMsg;
}

EngineException::~EngineException() { }

QString EngineException::getMessage() {
    return this->errorMsg;
}
