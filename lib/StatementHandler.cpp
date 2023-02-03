#include "StatementHandler.h"
#include "Append.h"
#include "FunctionDefinition.h"
#include "VariableDeclaration.h"
#include "Assignment.h"
#include "Plot.h"
#include "Set.h"
#include "Loop.h"
#include "If.h"
#include "Rotation.h"
#include "Done.h"
#include "Else.h"
#include "Parametric.h"
#include "Polar.h"

#ifdef ENABLE_PYTHON
#include "PythonScript.h"
#endif

#include <thread>
#include <mutex>

std::once_flag flag;

void registerClasses()
{
    std::call_once(flag, []() {

        nsAppend::HhAppend::create();
        nsFunctionDefinition::HhFunctionDefinition::create();
        nsVariableDeclaration::HhVariableDeclaration::create();
        nsAssignment::HhAssignment::create();
        nsPlot::HhPlot::create();
        nsSet::HhSet::create();
        nsLoop::HhLoop::create();
        nsIf::HhIf::create();
        nsRotation::HhRotation::create();
        nsDone::HhDone::create();
        nsElse::HhElse::create();
    #ifdef ENABLE_PYTHON
        nsPythonScript::HhPythonScript::create();
    #endif
        nsParametric::HhParametric::create();
        nsPolar::HhPolar::create();
    });
}

std::shared_ptr<AH> HandlerStore::getHandler(const QString &s)
{
    auto i = std::find_if(handlerClasses.begin(), handlerClasses.end(), [s](const auto& h) {return h->accepts(s); });
    return i == handlerClasses.end() ? nullptr : *i;
}
