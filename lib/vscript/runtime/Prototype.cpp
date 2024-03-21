﻿#include "vscript/runtime/Prototype.hpp"

#include "vscript/runtime/eval.hpp"

namespace vs::runtime
{

    Prototype::Prototype(const TSmartPtrType<ScopeLike>& scope) : DynamicObject(scope)
    {
        
    }

    Prototype::Prototype(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<NativeFunction>& func) : DynamicObject(scope)
    {
        DynamicObject::Set(ReservedDynamicFunctions::CALL,func);
    }

    std::string Prototype::ToString()
    {
        return "<Prototype>";
    }

    bool Prototype::ToBoolean() const
    {
        return true;
    }

    RuntimePrototype::RuntimePrototype(const TSmartPtrType<ScopeLike>& scope,
                                       const std::shared_ptr<parser::PrototypeNode>& prototype) : Prototype(scope)
    {
        _prototype = prototype;
        std::vector<std::string> args;
        for(auto &statement : _prototype->scope->statements)
        {
            if(statement->type == parser::NT_Function)
            {
                if(const auto fn = std::dynamic_pointer_cast<parser::FunctionNode>(statement))
                {
                    if(fn->name == ReservedDynamicFunctions::CONSTRUCTOR)
                    {
                        args = fn->args;
                        break;
                    }
                }
            }
        }
        
        AddNativeMemberFunction(ReservedDynamicFunctions::CALL,this,args,&RuntimePrototype::Create);
    }

    std::string RuntimePrototype::ToString()
    {
        return "<Prototype : " + _prototype->id + ">";
    }

    TSmartPtrType<Object> RuntimePrototype::Create(TSmartPtrType<FunctionScope>& fnScope)
    {
        auto dynamicObj = createDynamicFromPrototype(_prototype,GetScope());
        auto dynamicObjScope = dynamicObj->GetScope();

        if(dynamicObjScope->Has(ReservedDynamicFunctions::CONSTRUCTOR,false))
        {
            if(auto ctor = dynamicObjScope->Find(ReservedDynamicFunctions::CONSTRUCTOR,false).Cast<Function>(); ctor.IsValid() && ctor->GetType() == OT_Function)
            {
                ctor->HandleCall(fnScope);
            }
        }

        return dynamicObj;
    }

    TSmartPtrType<RuntimePrototype> makePrototype(const TSmartPtrType<ScopeLike>& scope,
                                                  const std::shared_ptr<parser::PrototypeNode>& prototype)
    {
        
        return manage<RuntimePrototype>(makeScope(scope),prototype);
    }

    TSmartPtrType<Prototype> makePrototype(const TSmartPtrType<ScopeLike>& scope,
        const TSmartPtrType<NativeFunction>& func)
    {
        return manage<Prototype>(makeScope(scope),func);
    }
}