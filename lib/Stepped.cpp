#include "Stepped.h"
#include "Function.h"

Stepped::Stepped() noexcept : step(Function::temporaryFunction("0.01", nullptr))
{

}
