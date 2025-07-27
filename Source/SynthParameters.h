#pragma once

#include <vector>

template<typename T>
class SynthParameters
{
public:
    SynthParameters() = default;
    ~SynthParameters();

    void add(T* parameterPtr);
    void clear();
    const std::vector<T*>& getAll() const;

private:
    std::vector<T*> parameters;
};