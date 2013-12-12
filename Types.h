#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

class RegisteredTypes_t
{
public:
    StrTo64Map_t* consts_;
    std::map<long long, size_t>* sizes_;
    MembVarMap_t* mvars_;
    VarMap_t* vars_;
    MemberFuncMap_t*  memberFuncs_;
    exception_data* expn_;
    int freeCode_;
    int* func_level_;

    bool Ok ()
    {
        return (consts_ &&
                sizes_ &&
                mvars_ &&
                vars_ &&
                memberFuncs_ &&
                expn_ &&
                func_level_);
    }

    RegisteredTypes_t (StrTo64Map_t* consts,
                       std::map<long long, size_t>* sizes,
                       MembVarMap_t* mvars,
                       VarMap_t* vars,
                       int* func_level,
                       MemberFuncMap_t*  memberFuncs,
                       exception_data* expn) :
        consts_      (consts),
        sizes_       (sizes),
        mvars_       (mvars),
        vars_        (vars),
        memberFuncs_ (memberFuncs),
        expn_        (expn),
        freeCode_    (TYPE_PTR + 1),
        func_level_  (func_level)
    {
        AddType ("byte", TYPE_BYTE, 1);
        AddType ("word", TYPE_WORD, 2);
        AddType ("dword", TYPE_DWORD, 4);
        AddType ("qword", TYPE_QWORD, 8);
        AddType ("ptr", TYPE_PTR, 4);
    }

    void AddType (std::string name, long long code, size_t size)
    {
        if (!Ok ()) return;
        std::string typename_ ("type_");
        typename_ += name;
        (*consts_)[typename_] = code;
        (*sizes_)[code] = size;
    }

    long long AddNewType (std::string name)
    {
        if (!Ok ()) return -1;
        std::string typename_ ("type_");
        typename_ += name;
        (*consts_)[typename_] = freeCode_;
        (*sizes_)[freeCode_] = 0;
        freeCode_++;
        return freeCode_ - 1;
    }

    bool AddVar (std::string var, long long struct_, long long type)
    {
        if (!Ok ()) return false;
        if (sizes_->find (struct_) == sizes_->end ()) return false;
        if (mvars_->find (struct_) == mvars_->end () ) (*mvars_)[struct_] = std::map<std::string, MemberVarDescriptor_t> ();
        (*mvars_)[struct_][var] = { (*mvars_)[struct_].size (), (*sizes_)[struct_], type};
        (*sizes_)[struct_] += (*sizes_)[type];
        return true;
    }

    bool AddFunc (std::string func, long long line, long long struct_)
    {
        if (!Ok ()) return false;
        auto found = memberFuncs_->end ();
        if ((found = memberFuncs_->find (func)) != memberFuncs_->end () &&
            found->second.func_ != -1) return false;
        ( (*memberFuncs_)[func]) = {line, struct_};
        return true;
    }

    bool ManageStructVar (char* flag, long long* arg, long long struct_ = 0)
    {
        if (!Ok ()) return false;
        if ((reinterpret_cast <std::string*> (*arg))->find ('.') == (reinterpret_cast <std::string*> (*arg))->npos
            && !struct_)
            return false;
        int nested = struct_ ? 1 : 0;
        long long offset = /*struct_ ? (*mvars_)[struct_][*(std::string*) (*arg)].offset :*/ 0;
        long long typeCode = 0;
        if (struct_ && !mvars_->empty ())
        {
            auto varsFnd = (*mvars_)[struct_].end ();
            if ((varsFnd = (*mvars_)[struct_].find (*reinterpret_cast<std::string*> (*arg))) != (*mvars_)[struct_].end ())
            {
                typeCode = varsFnd->second.typeCode;
            }
        }

        size_t pointOld = 0;
        size_t point = struct_ ? -1 : 0;
        long long var = 0;
        bool varSet = false;
        long long varCode = struct_;
        bool error = false;
        bool last = false;
        long long func = 0;
        bool isFunc = 0;
        while (!last)
        {
            pointOld = point;
            if (nested) pointOld++;
            point = (reinterpret_cast<std::string*> (*arg))->find ('.', pointOld);
            std::string token;
            if (point == (reinterpret_cast<std::string*> (*arg))->npos)
            {
                last = true;
                point = (reinterpret_cast<std::string*> (*arg))->size ();
            }
            for (size_t i = pointOld; i < point; i++) token += (*reinterpret_cast<std::string*> (*arg))[i];
            auto varsFound = vars_->end ();
            if (!nested &&
                (varsFound = vars_->find (StrTo32Pair_t (token, *func_level_))) != vars_->end ())
            {
                varSet = true;
                varCode = varsFound->second.typeCode;
                var = varsFound->second.num;
            }
            if (nested)
            {
                auto varsFound1 = (*mvars_)[varCode].end ();
                auto funcFound = memberFuncs_->end ();
                if ((varsFound1 = (*mvars_)[varCode].find (token)) != (*mvars_)[varCode].end ())
                {
                    offset += varsFound1->second.offset;
                    typeCode = varsFound1->second.typeCode;
                    if (! (*sizes_)[varsFound1->second.typeCode]) error = true;
                    //    NAT_EXCEPTION (expn_, "Internal error, size = 0", ERROR_NULL_SIZE_STRUCT)
                }
                else if ((funcFound = memberFuncs_->find (token)) != memberFuncs_->end ())
                {
                    func = funcFound->second.func_;
                    isFunc = true;
                }
                else
                {
                    /*static std::string str ("Member \"");
                    str += token;
                    str += "\" not found in structure";
                    NAT_EXCEPTION (expn_, str.c_str (), ERROR_MEMBER_NOT_FOUND)*/
                    error = true;
                }
                varCode = typeCode;
            }

            if (error) break;
            nested++;
        }
        //! 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
        //! |   typeCode   ||     num      ||           offset             |
        //! |              num             ||            line              |

        if (!error)
        {
            if (!varSet) var = -1;
            if (!isFunc)
            {
                if (*flag & ARG_UNREF_MASK)
                {
                    if (typeCode == TYPE_PTR) *flag = static_cast<char> (ARG_UNREF_MASK | ARG_VAR_MEMBER);
                    else NAT_EXCEPTION (expn_, "Invalid use of '*'", ERROR_INVALID_UNREF)
                }
                else *flag = ARG_VAR_MEMBER;
                delete reinterpret_cast<std::string*> (*arg);
                *arg = 0;
                *arg = short (typeCode);
                *arg <<= 16;
                *arg |= short (var + 1);
                *arg <<= 32;
                *arg |= int (offset);

            }
            else //! isFunc
            {
                if (*flag & ARG_UNREF_MASK)
                    NAT_EXCEPTION (expn_, "Invalid use of '*'", ERROR_INVALID_UNREF)
                if (func == -1)
                {
                    *flag = ARG_FUNC_MEMBER_IT;
                    *arg <<= 32;
                    *arg |= int (struct_);
                }
                else
                {
                    *flag = ARG_FUNC_MEMBER;
                    delete reinterpret_cast<std::string*> (*arg);
                    *arg = 0;
                    *arg |= int (var + 1);
                    *arg <<= 32;
                    *arg |= int (func);
                }
            }
        }
        if (!error && nested) return true;
        else return false;

    }
};

#endif // TYPES_H_INCLUDED
