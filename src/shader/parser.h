#include "../base.h"
#include <string>
#include <unordered_map>
#include <stack>
#include <assert.h>
#include <memory>

using namespace std;

struct argInfo
{
public:
     argInfo(int _typeId = 0, string _name = "", string _defValue = "")
    {
        type = _typeId;
        name = _name;
        defaultValue = _defValue;
    }
    //public bool isConst;
    int type;
    string name;
    string defaultValue;
};



struct baseFunc
{
public:
    string tempType = "";
    bool isOverride;
    vector<argInfo> args;
};

struct Constr : public baseFunc
{ };

struct Destruct : baseFunc
{
};


struct func : baseFunc
{
    int retType;
    string name;
    bool isConst;
};

struct Range
{
    Range(int start, int end)
    {
        this->Start = start;
        this->End = end;
    }
    int Start, End;
    /* data */
};
using psid = char;

enum ps : psid
{
    op = -1, // -n: next n symb is optional but only if there is actily this n symbols exsists
             // for example for smt {-2, word, _class, word, je};

    // "my class Uparser;"   = {word, _class, word, je}  =  {[word, _class,] word, je} = ok
    // "Uparser;"            = {word, je}                =  {[word, _class,] word, je} = ok
    // "my Uparser;"         = {word, word, je}          != {[word, _class,] word, je} = not ok

    // system symbols (can be only in the base str)
    _invalid = 0,
    _if = 1,            // {_if, n, m} = if next n symbls are exists, the next m symbols must be in the stmt else skip m symbls
    _ifel = 2,          // {_ifel, n, m, e} = if next n symbls are exists, the next m symbols must be in the stmt else 'e' symbls must be in the stmt
    repitable = 3,      // {repitable, n} = the next n symbols can be repited some times
    skipUntell = 4,     // {skipUntill, OP} = skip all words until the OP wont be found
    skipUntellSafe = 5,     // {skipUntillSafe, OP1, OP2} = e.g.{skipUntillSafe, '(', ')' } //  s + (3+4)) = OK (Last skob) // s + (3+4) = FAIL
    skip,               // skip next symbols check and return true
    end,                // end of stmt (must be in base and input strs)

    word,               //any word
    skip_word,          //any skip word

    //================== user def
    fig_skob_op, // {
    fig_skob_cl, // }

    pslast
};

template<class T>
struct psbase
{
    psid id;
    T token;
    bool sep = true; // токен целиком (s <token> s)
    /* data */
    psbase(psid id, T token)
    {
        this->token = token;
        this->id = id;
    }
};


struct psSkipword : public psbase<char>
{
    char secondToken;
    psSkipword(psid id, char firstToken, char secondToken) : psbase(id, firstToken)
    {
        this->secondToken = secondToken;
    }
};


using psSingleinfo = psbase<char>;
using psWordinfo = psbase<string>;

struct signature
{
private:
    ps* sign = nullptr;
    int _size;

public:
    signature()
    {}

    int size() const
    {
        return _size;
    }

    psid get(unsigned int index) const
    {
        if (index > _size)
            throw std::exception();

        return sign[index];
    }

    psid operator[](unsigned int index)
    {
        return get(index);
    }

    ~signature()
    {
        if (sign!= nullptr)
            {
                delete sign;
                sign = nullptr;
            }
    }
    /* data */
};

class parcInfo
{
    friend class Uparser;
    parcInfo()
    {}

    void pushPsRange(psid oper, int st, int ed)
    {
        pozs.push_back(Range(st, ed));
        sign.push_back(oper);
    }
public:
    string line;
    vector<psid> sign;
    vector<Range> pozs;


    Range* getRangeWhere(psid bc_ind, int start = 0)
    {
        for (; start < sign.size(); ++start)
        {
            if (sign[start] == bc_ind)
                return &pozs[start];
        }
        return nullptr;
    }

    psid getSign(int ind)
    {
        return sign[ind];
    }
    int getPozSt(int ind)
    {
        return pozs[ind].Start;
    }
    int getPozEd(int ind)
    {
        return pozs[ind].End;
    }

    string getSub(int ind)
    {
        return line.substr(getPozSt(ind), getPozEd(ind));
    }

    bool isSign(int poz, psid pc_ind)
    {
        return sign[poz] == pc_ind;
    }

    int getLen()
    {
        int i = 0;
        while (/*i < sign.Length &&*/ sign[i++] != ps::end) ;
        return i;
    }
    Range getLast()
    {
        return pozs[getLen() - 1];
    }
    bool contains(psid je)

    {
        int i = 0;
        while (/*i < sign.Length &&*/ sign[i] != ps::end)
        {
            if (sign[i++] == je)
                return true;
        }
        return false;
    }

    // string getFullType(ref int startnd)
    // {
    //     // [const] void [< T >] [*]  name
    //     int couMun = 0;
    //     while (true)
    //     {
    //         if (isSign(startnd, ps::word))
    //         {
    //             couMun++;
    //             if (couMun == 2)
    //                 break;
    //         }
    //         ++startnd;
    //     }
    //     // skip [const]
    //     return line[(isSign(0, ps::_const) ? getPozSt(1) : 0)..(getPozSt(startnd) - 1)];
    // }
};


struct savePair
{
    int baseInd = 0;
    int inpInd = 0;
    savePair(int _baseInd = 0, int _inpInd = 0)
    {
        this->baseInd = _baseInd;
        this->inpInd = _inpInd;
    }
};

class Uparser
{
private:
    psid glob_id;
    unordered_map<string, psWordinfo> wordParser;
    unordered_map<char, psSingleinfo> singleParser;
    unordered_map<char, psSkipword> skipWordParser;
    // vector<psSingleinfo> singleTokens;
    // vector<psWordinfo> wordTokens;
    // vector<psSkipword> skipWordTokens;
    std::unique_ptr<parcInfo> lineInfo;
    int userDefStart = 0;

public:
    Uparser()
    {
        // tokens.push_back(psSingleinfo(ps::zero, '0'));
        // tokens.push_back(psSingleinfo(ps::ravn, '='));
        // singleTokens.push_back(psSingleinfo(ps::fig_skob_op, '{'));
        // singleTokens.push_back(psSingleinfo(ps::fig_skob_cl, '}'));
        // addSingleToken()
        // singleTokens.push_back(psSingleinfo(ps::je, ';'));
        
        // skipWordTokens.push_back(psSkipword(glob_id++, '"', '"'));
        // skipWordTokens.push_back(psSkipword(glob_id++, '\'', '\''));
        // skipWordTokens.push_back(psSkipword(glob_id++, '(', ')'));
        glob_id = ps::pslast + 1;
    }
    // returns assigned token id
    int addWordToken(string token, int id = 0)
    {
        if (id==0)
            id = glob_id++;

        wordParser.insert(pair<string, psWordinfo>(token, psWordinfo(id, token)));

        return id;
    }

    int addSingleToken(char token, int id = 0)
    {
        if (id==0)
            id = glob_id++;

        singleParser.insert(pair<char, psSingleinfo>(token, psSingleinfo(id, token)));

        return id;
    }

    int addSkipWordToken(char firstToken, char secondToken, int id = 0)
    {
        if (id==0)
            id = glob_id++;

        skipWordParser.insert(pair<char, psSkipword>(firstToken, psSkipword(id, firstToken, secondToken)));

        return id;
    }

    bool compireSigs(const signature& baseSig, const signature& inputSig)
    {
        //if (inputSig. > baseSig.Length)
        //    return false;
        savePair ind;

        std::stack<savePair> savePoint;

        while (true)
        {
            if (ind.baseInd >= baseSig.size())
                return false;

            if (ind.inpInd >= inputSig.size())
                return false;

            if (baseSig.get(ind.baseInd) == ps::op)
            {
                savePoint.push(savePair(ind.baseInd + 2, ind.inpInd));
                ++ind.baseInd;
            }


            psid baseChar = baseSig.get(ind.baseInd);
            psid inpChar = inputSig.get(ind.inpInd);
            // assert((int)inpChar != 0);

            if (baseChar == ps::skip)
                return true;

            if (baseChar == inpChar)
            {
                if (inpChar == ps::end)
                    return true;

                ind.baseInd++;
                ind.inpInd++;
            }
            else
            {
                if (savePoint.size() > 0)
                {
                    ind = savePoint.top();
                    savePoint.pop();
                    continue;
                }
                else
                    return false;
            }
        }
    }

private:
    void closeWord(parcInfo* inLineInf, int workSt, int poz, bool initNew = false)
    {
        if (workSt != -1)
        {
            string wordstr = inLineInf->line.substr(workSt, poz);

            inLineInf->pozs.push_back(Range(workSt, poz));
            auto codeRet = wordParser.find(wordstr);
            if (codeRet != wordParser.end())
                inLineInf->sign.push_back(codeRet->second.id);
            else
                inLineInf->sign.push_back(ps::word);

            workSt = -1;
            //word = false;

        }
        else if (initNew)
            workSt = poz;
    }

    int getCharType(char base)
    {
        if (isdigit(base))
        {
            return 1;
        }
        if (isalpha(base))
        {
            return 2;
        }
        if (base == ' ' || base == '\t' || base == '\n' || base == '\r')
        {
            return 3;
        }

        return 4;
    }

    bool checkSeparateChar(char base, char another)
    {
        return getCharType(base) != getCharType(another);
    }

    parcInfo *getInnerSign(string line, bool &multiComment)
    {


        parcInfo* inLineInf;
        inLineInf->line = line;
        // inLineInf.sign = new psid[30];
        // inLineInf.pozs = new Range[30];

        int curl = 0;
        //bool word = false;
        int workSt = -1;
        multiComment = false;

        bool sterStarts = true;

        for (int i = 0; i < line.length(); i++)
        {
            char c = line[i];
            switch (c)
            {
            case '/':
                if (i +1 < line.length())
                {
                    if (line[i + 1] == '/')
                        break;

                    if (line[i + 1] == '*')
                    {
                        multiComment = true;
                        break;
                    }
                }
                break;//und

            case '\t':
            case ' ':
            case '\n':
            case '\r':
                closeWord(inLineInf, workSt, i);
                continue;

        }

        auto ftoken = singleParser.find(c);

        if (ftoken != singleParser.end())
        {
            if (ftoken->second.sep)
            {
                bool sep = true;
                if (i>0)
                    sep &= checkSeparateChar(line[i], line[i - 1]);
                if (i + 1 < line.size())
                    sep &= checkSeparateChar(line[i], line[i + 1]);

                if (!sep)
                    continue;
            }
            closeWord(inLineInf, workSt, i);
            inLineInf->pushPsRange(ftoken->second.id, i, i + 1);
            break;
        }

        // "dssdds "
        int ccount = 0;
        auto stoken = skipWordParser.find(c);
        {
            int st = i;
            while (i < line.length() && (line[i] != c || ccount != 1))
            {
                if (line[i] == c)
                    ++ccount;

                if (line[i] == stoken->second.secondToken)
                    --ccount;

                ++i;
            }
            inLineInf->pushPsRange(ftoken->second.id, st, i);

            break;
        }
        if (workSt == -1)
            workSt = i;
        }

        closeWord(inLineInf, workSt, line.length());
        inLineInf->pushPsRange(ps::end, line.length(), line.length());
        return inLineInf;
    }

public:

    string pLine;
    bool processLine(string line, bool& multiComment)
    {
        pLine = line;
        lineInfo.reset(getInnerSign(line, multiComment));
        return static_cast<bool>(lineInfo);
    }

    //     void addConst(bool op)
    //     {
    //         addOp(op);
    //         sign.push_back(ps::_const);
    //     }
    //     void addEnd()
    //     {
    //         sign.push_back(ps::end);
    //     }

    //     void add(psid sim)
    //     {
    //         sign.push_back(sim);
    //     }

    //     static psid[] getAbstructFuncpc()
    //     {
    //         //[virual] [const] void [<T>] [*]  v(...) [const] [override] [;] = 0;

    //         sigconstr cont = new sigconstr();
    //         cont.addVirtual(true);
    //         cont.addType();
    //         cont.add(ps::word);
    //         cont.add(ps::skob_word);
    //         cont.addConst(true);
    //         cont.addInitZero();
    //         cont.addje(false);
    //         cont.addEnd();
    //         return cont.sign.ToArray();
    //     }
    //};

    // bool isConstr()
    // {
    //     // v(...) [;]
    //     psid[] funcFig = sigconstr.getConstrpc();
    //     return compireSigs(funcFig, lineInfo.sign);
    // }
};



// static string getNextWord(string& line, int start = 0)
// {
//     if (line.length() == 0)
//         return line;

//     while (start < line.length() && !isalpha(line[start++])) ;

//     int end = start--;
//     while (end < line.length() && isalpha(line[end++])) ;
//     end--;

//     string word = line.substr(start, end);
//     line = line.substr(end + 1);
//     return word;
// }

// class FileReader
// {
//     string pathToSource;
//     // vector<classInfo> classes;
//     // unordered_map<string, int> types ;

//     static bool check(string a)
//     {
//         return a.length() != 0;
//     }

//     static bool check(baseFunc a)
//     {
//         return a != null;
//     }

//     static bool check(argInfo a)
//     {
//         return a != null;
//     }
//     static bool check(classInfo a)
//     {
//         return a != null;
//     }

//     static void analysFile(string filepath)
//     {
//         StreamReader sr = new StreamReader(filepath);
//         string line;
//         bool avlZone = false;
//         bool startClass = false;

//         classInfo classInfo = null;

//         Uparser Uparser ();
//         int skobs = 0;
//         int funSkob = -1, skobsCountWhereClassStarts = -1;
//         bool multiComm = false;


//         void checkSkobsFnd(psid neededSkob, ref int initOfFnd)
//         {
//             psid curTe = Uparser.lineInfo.getSign(0);
//             int inde = 1;
//             int sd = skobs;
//             while (curTe != ps::end)
//             {
//                 if (curTe == neededSkob)
//                     initOfFnd = skobs;
//                 if (curTe == ps::fig_skob_op)
//                     skobs++;
//                 else if (curTe == ps::fig_skob_cl)
//                     skobs--;
//                 curTe = Uparser.lineInfo.getSign(inde++);
//             }

//             if (skobs == sd)
//                 return;
//             if (skobsCountWhereClassStarts != -1)
//             {
//                 if (skobs < skobsCountWhereClassStarts)
//                 {
//                     skobsCountWhereClassStarts = -1;
//                     classInfo = nullptr;
//                 }
//                 // !!!!!!!!!!!!!!!!!!
//             }
//             if (funSkob != -1)
//             {
//                 if (skobs <= funSkob)
//                     funSkob = -1;
//             }
//         }

//         void checkSkobs()
//         {
//             psid curTe = Uparser.lineInfo.getSign(0);
//             int inde = 1;
//             int startCount = skobs;
//             while (curTe != ps::end)
//             {
//                 if (curTe == ps::fig_skob_op)
//                     skobs++;
//                 else if (curTe == ps::fig_skob_cl)
//                     skobs--;
//                 curTe = Uparser.lineInfo.getSign(inde++);
//             }
//             if (startCount <= skobs)
//                 return;

//             if (skobsCountWhereClassStarts != -1)
//             {
//                 if (skobs < skobsCountWhereClassStarts)
//                 {
//                     skobsCountWhereClassStarts = -1;
//                     classInfo = null;
//                 }
//                 // !!!!!!!!!!!!!!!!!!
//             }
//             if (funSkob != -1)
//             {
//                 if (skobs < funSkob)
//                     funSkob = -1;
//             }
//         }


//         int zone = 0;

//         const int classZone = 5;
//         const int funcZone = 6;

//         string templ = "";
//         while ((line = sr.ReadLine()) != null)
//         {
//             line = line.Trim();
//             if (multiComm)
//             {
//                 int indcom = line.IndexOf("*/");
//                 if (indcom == -1)
//                     continue;

//                 line = line[(indcom + 2)..];
//             }
//             while (line.EndsWith(','))
//             {
//                 line += sr.ReadLine().Trim();
//             }

//             if (line.Contains("delete[] hist;"))
//                 cout << ();
//             if (!Uparser.getSignature(line, out multiComm))
//                 continue;

//             // TEMPLATE
//             string temp = Uparser.getTemplate();
//             if (check(temp))
//             {
//                 //Debug.Assert(templ.Length == 0);
//                 templ = temp;
//                 continue;
//             }

//             string clas = Uparser.getClassProto();
//             if (check(clas))
//             {
//                 templ = "";
//                 getTypeId(clas);
//                 checkSkobs();
//                 continue;
//             }

//             clas = Uparser.getClass();
//             if (check(clas))
//             {
//                 classInfo = new classInfo
//                 {
//                     name = clas
//                 };
//                 getTypeId(clas);

//                 classes.Add(classInfo);

//                 avlZone = false;
//                 //skobs = 0;
//                 if (check(templ))
//                 {
//                     classInfo.tempType = templ;
//                     templ = "";
//                 }
//                 //skobsCountWhereClassStarts = 0;
//                 skobsCountWhereClassStarts = skobs + 1;
//                 checkSkobs();
//                 //checkSkobsFnd(ps::pclass, ref skobsCountWhereClassStarts);
//                 continue;
//             }

//             if (Uparser.isUsing())
//             {
//                 templ = "";
//                 continue;
//             }
//             checkZone(line, ref avlZone);


//             if (check(classInfo))
//             {

//                 if (avlZone && (funSkob == -1) && skobs == skobsCountWhereClassStarts)
//                 {
//                     var membr = Uparser.getMember();
//                     if (check(membr))
//                     {
//                         int mem = 0;
//                         classInfo.members.Add(membr);

//                         checkSkobs();
//                         continue;
//                     }

//                     var conct = Uparser.getConstr(classInfo.name);
//                     if (check(conct))
//                     {
//                         if (!Uparser.lineInfo.contains(ps::je))
//                         {
//                             // Есть тело
//                             funSkob = skobs + 1;
//                         }
//                         classInfo.consrts.Add(conct);
//                         checkSkobs();
//                         continue;
//                     }

//                     var destr = Uparser.getDestruct(classInfo.name);
//                     if (check(destr))
//                     {
//                         if (!Uparser.lineInfo.contains(ps::je))
//                         {
//                             // Есть тело
//                             funSkob = skobs + 1;
//                         }
//                         Debug.Assert(classInfo.destructor == null);
//                         classInfo.destructor = destr;
//                         checkSkobs();
//                         continue;
//                     }


//                     //"bc::BarRoot<T>* getRootNode()"
//                     var func = Uparser.getFunc();
//                     if (check(func))
//                     {
//                         if (check(templ))
//                         {
//                             func.tempType = templ;
//                             templ = "";
//                         }

//                         classInfo.funcs.Add(func);
//                         if (!Uparser.lineInfo.contains(ps::je))
//                         {
//                             // Есть тело
//                             funSkob = skobs + 1;
//                         }
//                         checkSkobs();
//                         continue;
//                     }
//                 }
//             }

//             checkSkobs();
//         }
//     }
// };
