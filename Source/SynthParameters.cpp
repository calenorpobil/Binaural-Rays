#pragma once

#include "SynthParameters.h"

template<typename T>
SynthParameters<T>::~SynthParameters()
{
    clear();
}

template<typename T>
void SynthParameters<T>::add(T* parameterPtr)
{
    parameters.push_back(parameterPtr);
}

template<typename T>
void SynthParameters<T>::clear()
{
    parameters.clear(); // No borra los punteros. Si quieres borrarlos, haz un delete
}

template<typename T>
const std::vector<T*>& SynthParameters<T>::getAll() const
{
    return parameters;
}
