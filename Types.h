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
        freeCode_    (TYPE_QWORD + 1),
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
        if (!Ok()) return;
        std::string typename_ ("type_");
        typename_ += name;
        (*consts_)[typename_] = code;
        (*sizes_)[code] = size;
    }

    long long AddNewType (std::string name)
    {
        if (!Ok()) return -1;
        std::string typename_ ("type_");
        typename_ += name;
        (*consts_)[typename_] = freeCode_;
        (*sizes_)[freeCode_] = 0;
        freeCode_++;
        //printf ("Added type %s %d\n", name.c_str(), freeCode_ - 1);
        return freeCode_ - 1;
    }

    bool AddVar (std::string var, long long struct_, long long type)
    {
        //printf ("CALLED ADDVAR _____________________\n");
        if (!Ok()) return false;
        if (sizes_->find (struct_) == sizes_->end()) return false;
        if (mvars_->find (struct_) == mvars_->end() )(*mvars_)[struct_] = std::map<std::string, MemberVarDescriptor_t> ();
        (*mvars_)[struct_][var] = {(*mvars_)[struct_].size(), (*sizes_)[struct_], type};
        (*sizes_)[struct_] += (*sizes_)[type];
        //printf ("ADDING VAR %s, %lld, %d\n", var.c_str(), (*mvars_)[struct_][var].offset, (*sizes_)[struct_]);
        return true;
    }

    bool AddFunc (std::string func, long long line, long long struct_)
    {
        //printf ("ADDING FUNC %s, %lld\n", func.c_str(), line);
        if (!Ok()) return false;
        auto found = memberFuncs_->end();
        if ((found = memberFuncs_->find(func)) != memberFuncs_->end() &&
            found->second.func_ != -1) return false;
        ((*memberFuncs_)[func]) = {line, struct_};
        //printf ("");
        return true;
    }

    bool ManageStructVar (char* flag, long long* arg, long long struct_ = 0)
    {
        //printf ("YOO\n");
        if (!Ok()) return false;
        //printf ("A %s, %d\n", ((std::string*)(*arg))->c_str(), ((std::string*)(*arg))->npos);
        if (((std::string*)(*arg))->find ('.') == ((std::string*)(*arg))->npos && !struct_)
            return false;
        //printf ("B\n");
        int nested = struct_ ? 1 : 0;
        //printf ("C\n");
        long long offset = /*struct_ ? (*mvars_)[struct_][*(std::string*)(*arg)].offset :*/ 0;
        //printf ("D\n");
        //printf ("AAAAAAAAAAAAAA %d\n", mvars_->size());
        long long typeCode = (struct_ && !mvars_->empty()) ? (*mvars_)[struct_][*(std::string*)(*arg)].typeCode : 0;
        //printf ("E\n");
        //printf ("AAAAAAAAAAAAAA %d\n", mvars_->size());
        size_t pointOld = 0;
        //printf ("F\n");
        size_t point = struct_ ? -1 : 0;
        //printf ("G\n");
        long long var = 0;
        //printf ("H\n");
        bool varSet = false;
        //printf ("I\n");
        long long varCode = struct_;
        //printf ("J\n");
        bool error = false;
        //printf ("K\n");
        bool last = false;
        //printf ("L\n");
        long long func = 0;
        //printf ("M\n");
        bool isFunc = 0;
        //printf ("A |%s|\n", ((std::string*)(*arg))->c_str());
        //printf ("AAA, %d\n", struct_);
        while (!last)
        {
            pointOld = point;
            if (nested) pointOld++;
            //printf ("POINTOLD %d\n", pointOld);
            //if (nested && (!struct_ || nested > 1)) pointOld++;
            point = ((std::string*)(*arg))->find ('.', pointOld);
            std::string token;
            if (point == ((std::string*)(*arg))->npos)
            {
                last = true;
                point = ((std::string*)(*arg))->size();
            }
            //printf ("B\n");
            for (size_t i = pointOld; i < point; i++) token += (*(std::string*)(*arg))[i];
            auto varsFound = vars_->end();
            if (!nested &&
                (varsFound = vars_->find(StrTo32Pair_t(token, *func_level_))) != vars_->end())
            {
                varSet = true;
                varCode = varsFound->second.typeCode;
                var = varsFound->second.num;
            }
            //printf ("C %s\n", token.c_str());
            if (nested)
            {
                //printf ("TEST %s\n", token.c_str());
                auto varsFound1 = (*mvars_)[varCode].end();
                auto funcFound = memberFuncs_->end();
                if ((varsFound1 = (*mvars_)[varCode].find(token)) != (*mvars_)[varCode].end())
                {
                    offset += varsFound1->second.offset;
                    typeCode = varsFound1->second.typeCode;
                    //printf ("AAAAAAAAAAAAAA %s %d\n", token.c_str(), mvars_->size());
                    if (!(*sizes_)[varsFound1->second.typeCode])
                        NAT_EXCEPTION (expn_, "Internal error, size = 0", ERROR_NULL_SIZE_STRUCT)
                }
                else if ((funcFound = memberFuncs_->find(token)) != memberFuncs_->end())
                {
                    func = funcFound->second.func_;
                    isFunc = true;
                }
                else
                {
                    //printf ("ERROR %s\n", token.c_str());
                    static std::string str ("Member \"");
                    str += token;
                    str += "\" not found in structure";
                    NAT_EXCEPTION (expn_, str.c_str(), ERROR_MEMBER_NOT_FOUND)
                }
                varCode = typeCode;
            }
            //printf ("D\n");

            if (error) break;
            nested++;
        }
        //! 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
        //! |   typeCode   ||     num      ||           offset             |
        //! |              num             ||            line              |

        if (!error)
        {
            /*delete (std::string*)(*arg);
            *arg = 0;*/
            if (!varSet) var = -1;
            if (!isFunc)
            {
                if (*flag & ARG_UNREF_MASK)
                {
                    if (typeCode == TYPE_PTR) *flag = ARG_UNREF_MASK | ARG_VAR_MEMBER;
                    else NAT_EXCEPTION (expn_, "Invalid use of '*'", ERROR_INVALID_UNREF)
                }
                else *flag = ARG_VAR_MEMBER;
                delete (std::string*)(*arg);
                *arg = short (typeCode);
                *arg <<= 16;
                *arg |= short (var + 1);
                *arg <<= 32;
                *arg |= int (offset);
            }
            else
            {
                if (*flag & ARG_UNREF_MASK)
                    NAT_EXCEPTION (expn_, "Invalid use of '*'", ERROR_INVALID_UNREF)
                if (func == -1)
                {
                    *flag = ARG_FUNC_MEMBER_IT;
                    *arg <<= 32;
                    *arg |= int(struct_);
                }
                else
                {
                    *flag = ARG_FUNC_MEMBER;
                    delete (std::string*)(*arg);
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
