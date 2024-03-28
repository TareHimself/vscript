﻿#include "vscript/frontend/Program.hpp"

#include <fstream>

#include "vscript/api.hpp"
#include "vscript/backend/Tokenizer.hpp"
#include "vscript/frontend/Error.hpp"
#include "vscript/frontend/eval.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{
    Program::Program() : DynamicObject({})
    {
        
    }

    void Program::OnRefSet()
    {
        DynamicObject::OnRefSet();
        AddLambda("import",{"moduleId"},[this](const TSmartPtrType<FunctionScope>& scope)
         {
            if(const auto mod = ImportModule(scope->Find("moduleId")->ToString()); mod.IsValid())
            {
                return mod.CastStatic<Object>();
            }

            return makeNull().CastStatic<Object>();
         });
        
        AddLambda("cwd",{},[this](const TSmartPtrType<FunctionScope>& fs)
        {
            return makeString(std::string(_cwd.string()));
        });

        DynamicObject::Set("List",List::Prototype);
    }

    TSmartPtrType<Module> Program::ImportModule(const std::string& id)
    {
        if(_modules.contains(id))
        {
            return _modules[id];
        }

        const auto absPath = absolute(std::filesystem::path(id));
        auto targetPath = absPath.string();
        targetPath = targetPath.substr(0,targetPath.size() - absPath.extension().string().size());

        if(_modules.contains(targetPath))
        {
            return _modules[targetPath];
        }

        if(!std::filesystem::exists(absPath))
        {
            throw std::runtime_error("File does not exist");
        }

        if(absPath.extension() == ".vsnative")
        {
            auto scope = this->ToRef().Reserve().Cast<Program>();
            auto native = api::importNative(absPath,scope);
            if(native.IsValid())
            {
                _modules[targetPath] = native;
            }
            return native;
        }
        
        auto mod = ModuleFromFile(absPath);

        if(mod.IsValid())
        {
            _modules[targetPath] = mod;
        }
        
        return mod;
    }

    TSmartPtrType<Module> Program::ModuleFromFile(const std::filesystem::path& path)
    {
        
        
        backend::Tokenized tokens;
        tokenize(tokens,path);

        for(auto &token : tokens.GetList())
        {
            token.debugInfo.file = path.string();
        }

        const auto ast = parse(tokens);

        return evalModule(ast,this->ToRef().Reserve().Cast<Program>());
    }

    TSmartPtrType<Module> Program::ImportModule(TSmartPtrType<FunctionScope>& scope, const std::string& id)
    {

        return ImportModule(id);
    }

    TSmartPtrType<Object> Program::Find(const std::string& id, bool searchParent)
    {
        if(id == "program")
        {
            return this->ToRef().Reserve();
        }
        
        return DynamicObject::Find(id, searchParent);
    }

    TSmartPtrType<Program> makeProgram()
    {
        return manage<Program>();
    }
}
