/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include <iostream>
#include "token.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

inline void Foam::token::clear()
{
    if (type_ == WORD)
    {
        delete wordTokenPtr_;
    }
    else if (type_ == FUNCTIONNAME)
    {
        delete functionNameTokenPtr_;
    }
    else if (type_ == VARIABLE)
    {
        delete variableTokenPtr_;
    }
    else if (type_ == STRING)
    {
        delete stringTokenPtr_;
    }
    else if (type_ == VERBATIMSTRING)
    {
        delete verbatimStringTokenPtr_;
    }
    else if (type_ == LONG_DOUBLE_SCALAR)
    {
        delete longDoubleScalarTokenPtr_;
    }
    else if (type_ == COMPOUND)
    {
        if (compoundTokenPtr_->unique())
        {
            delete compoundTokenPtr_;
        }
        else
        {
            compoundTokenPtr_->refCount::operator--();
        }
    }

    type_ = UNDEFINED;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::token::token()
:
    type_(UNDEFINED),
    lineNumber_(0)
{}


inline Foam::token::token(const token& t)
:
    type_(t.type_),
    lineNumber_(t.lineNumber_)
{
    switch (type_)
    {
        case token::UNDEFINED:
        break;

        case PUNCTUATION:
            punctuationToken_ = t.punctuationToken_;
        break;

        case WORD:
            wordTokenPtr_ = new word(*t.wordTokenPtr_);
        break;

        case FUNCTIONNAME:
            functionNameTokenPtr_ = new functionName(*t.functionNameTokenPtr_);
        break;

        case VARIABLE:
            variableTokenPtr_ = new variable(*t.variableTokenPtr_);
        break;

        case STRING:
            stringTokenPtr_ = new string(*t.stringTokenPtr_);
        break;

        case VERBATIMSTRING:
            verbatimStringTokenPtr_ =
                new verbatimString(*t.verbatimStringTokenPtr_);
        break;

        case INTEGER_32:
            integer32Token_ = t.integer32Token_;
        break;

        case INTEGER_64:
            integer64Token_ = t.integer64Token_;
        break;

        case UNSIGNED_INTEGER_32:
            unsignedInteger32Token_ = t.unsignedInteger32Token_;
        break;

        case UNSIGNED_INTEGER_64:
            unsignedInteger64Token_ = t.unsignedInteger64Token_;
        break;

        case FLOAT_SCALAR:
            floatScalarToken_ = t.floatScalarToken_;
        break;

        case DOUBLE_SCALAR:
            doubleScalarToken_ = t.doubleScalarToken_;
        break;

        case LONG_DOUBLE_SCALAR:
            longDoubleScalarTokenPtr_ =
                new longDoubleScalar(*t.longDoubleScalarTokenPtr_);
        break;

        case COMPOUND:
            compoundTokenPtr_ = t.compoundTokenPtr_;
            compoundTokenPtr_->refCount::operator++();
        break;

        case token::ERROR:
        break;
    }
}


inline Foam::token::token(punctuationToken p, label lineNumber)
:
    type_(PUNCTUATION),
    punctuationToken_(p),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const word& w, label lineNumber)
:
    type_(WORD),
    wordTokenPtr_(new word(w)),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const string& s, label lineNumber)
:
    type_(STRING),
    stringTokenPtr_(new string(s)),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const verbatimString& vs, label lineNumber)
:
    type_(VERBATIMSTRING),
    verbatimStringTokenPtr_(new verbatimString(vs)),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const int32_t l, label lineNumber)
:
    type_(INTEGER_32),
    integer32Token_(l),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const int64_t l, label lineNumber)
:
    type_(INTEGER_64),
    integer64Token_(l),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const uint32_t l, label lineNumber)
:
    type_(UNSIGNED_INTEGER_32),
    unsignedInteger32Token_(l),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const uint64_t l, label lineNumber)
:
    type_(UNSIGNED_INTEGER_64),
    unsignedInteger64Token_(l),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const floatScalar s, label lineNumber)
:
    type_(FLOAT_SCALAR),
    floatScalarToken_(s),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const doubleScalar s, label lineNumber)
:
    type_(DOUBLE_SCALAR),
    doubleScalarToken_(s),
    lineNumber_(lineNumber)
{}


inline Foam::token::token(const longDoubleScalar s, label lineNumber)
:
    type_(LONG_DOUBLE_SCALAR),
    longDoubleScalarTokenPtr_(new longDoubleScalar(s)),
    lineNumber_(lineNumber)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

inline Foam::token::~token()
{
    clear();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline Foam::token::tokenType  Foam::token::type() const
{
    return type_;
}

inline Foam::token::tokenType&  Foam::token::type()
{
    return type_;
}

inline bool Foam::token::good() const
{
    return (type_ != ERROR && type_ != UNDEFINED);
}

inline bool Foam::token::undefined() const
{
    return (type_ == UNDEFINED);
}

inline bool Foam::token::error() const
{
    return (type_ == ERROR);
}

inline bool Foam::token::isPunctuation() const
{
    return (type_ == PUNCTUATION);
}

inline Foam::token::punctuationToken  Foam::token::pToken() const
{
    if (type_ == PUNCTUATION)
    {
        return punctuationToken_;
    }
    else
    {
        parseError("punctuation character");
        return NULL_TOKEN;
    }
}

inline bool Foam::token::isWord() const
{
    return (type_ == WORD);
}

inline const Foam::word& Foam::token::wordToken() const
{
    if (type_ == WORD)
    {
        return *wordTokenPtr_;
    }
    else
    {
        parseError(word::typeName);
        return word::null;
    }
}

inline bool Foam::token::isFunctionName() const
{
    return (type_ == FUNCTIONNAME);
}

inline const Foam::functionName& Foam::token::functionNameToken() const
{
    if (type_ == FUNCTIONNAME)
    {
        return *functionNameTokenPtr_;
    }
    else
    {
        parseError(functionName::typeName);
        return functionName::null;
    }
}

inline bool Foam::token::isVariable() const
{
    return (type_ == VARIABLE);
}

inline const Foam::variable& Foam::token::variableToken() const
{
    if (type_ == VARIABLE)
    {
        return *variableTokenPtr_;
    }
    else
    {
        parseError(variable::typeName);
        return variable::null;
    }
}

inline bool Foam::token::isString() const
{
    return (type_ == STRING);
}

inline const Foam::string& Foam::token::stringToken() const
{
    if (type_ == STRING)
    {
        return *stringTokenPtr_;
    }
    else
    {
        parseError(string::typeName);
        return string::null;
    }
}

inline bool Foam::token::isVerbatimString() const
{
    return (type_ == VERBATIMSTRING);
}

inline const Foam::verbatimString& Foam::token::verbatimStringToken() const
{
    if (type_ == VERBATIMSTRING)
    {
        return *verbatimStringTokenPtr_;
    }
    else
    {
        parseError(verbatimString::typeName);
        return verbatimString::null;
    }
}

inline bool Foam::token::isAnyString() const
{
    return
    (
        type_ == WORD
     || type_ == FUNCTIONNAME
     || type_ == VARIABLE
     || type_ == STRING
     || type_ == VERBATIMSTRING
    );
}

inline const Foam::string& Foam::token::anyStringToken() const
{
    if (type_ == WORD)
    {
        return *wordTokenPtr_;
    }
    else if (type_ == FUNCTIONNAME)
    {
        return *functionNameTokenPtr_;
    }
    else if (type_ == VARIABLE)
    {
        return *variableTokenPtr_;
    }
    else if (type_ == STRING)
    {
        return *stringTokenPtr_;
    }
    else if (type_ == VERBATIMSTRING)
    {
        return *verbatimStringTokenPtr_;
    }
    else
    {
        parseError(string::typeName);
        return string::null;
    }
}

inline bool Foam::token::isInteger32() const
{
    return
        type_ == INTEGER_32
     || (
            type_ == INTEGER_64
         && (integer64Token_ >= INT32_MIN) && (integer64Token_ <= INT32_MAX)
        )
     || (type_ == UNSIGNED_INTEGER_32 && unsignedInteger32Token_ <= INT32_MAX)
     || (type_ == UNSIGNED_INTEGER_64 && unsignedInteger64Token_ <= INT32_MAX);
}

inline int32_t Foam::token::integer32Token() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<int32_t>::typeName);
        return 0;
    }
}

inline bool Foam::token::isInteger64() const
{
    return
        type_ == INTEGER_32
     || type_ == INTEGER_64
     || type_ == UNSIGNED_INTEGER_32
     || (type_ == UNSIGNED_INTEGER_64 && unsignedInteger64Token_ <= INT64_MAX);
}

inline int64_t Foam::token::integer64Token() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<int64_t>::typeName);
        return 0;
    }
}

inline bool Foam::token::isUnsignedInteger32() const
{
    return
        (type_ == INTEGER_32 && integer32Token_ >= 0)
     || (
            type_ == INTEGER_64
         && (integer64Token_ >= 0) && (integer64Token_ <= UINT32_MAX)
        )
     || type_ == UNSIGNED_INTEGER_32
     || (type_ == UNSIGNED_INTEGER_64 && unsignedInteger64Token_ <= UINT32_MAX);
}

inline uint32_t Foam::token::unsignedInteger32Token() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<uint32_t>::typeName);
        return 0;
    }
}

inline bool Foam::token::isUnsignedInteger64() const
{
    return
        (type_ == INTEGER_32 && integer32Token_ >= 0)
     || (type_ == INTEGER_64 && integer64Token_ >= 0)
     || type_ == UNSIGNED_INTEGER_32
     || type_ == UNSIGNED_INTEGER_64;
}

inline uint64_t Foam::token::unsignedInteger64Token() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<uint64_t>::typeName);
        return 0;
    }
}

inline bool Foam::token::isLabel() const
{
    return
        type_ == INTEGER_32
     || (
            type_ == INTEGER_64
         && integer64Token_ >= int64_t(labelMin)
         && integer64Token_ <= int64_t(labelMax)
        )
     || (
            type_ == UNSIGNED_INTEGER_32
         && uint64_t(unsignedInteger32Token_) <= uint64_t(labelMax)
        )
     || (
            type_ == UNSIGNED_INTEGER_64
         && unsignedInteger64Token_ <= uint64_t(labelMax)
        );
}

inline Foam::label Foam::token::labelToken() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<label>::typeName);
        return 0;
    }
}

inline bool Foam::token::isULabel() const
{
    return
        (
            type_ == INTEGER_32
         && integer32Token_ >= 0
        )
     || (
            type_ == INTEGER_64
         && integer64Token_ >= 0
         && uint64_t(integer64Token_) <= uint64_t(uLabelMax)
        )
     || type_ == UNSIGNED_INTEGER_32
     || (
            type_ == UNSIGNED_INTEGER_64
         && unsignedInteger64Token_ <= uint64_t(uLabelMax)
        );
}

inline Foam::uLabel Foam::token::uLabelToken() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else
    {
        parseError(pTraits<uLabel>::typeName);
        return 0;
    }
}

inline bool Foam::token::isFloatScalar() const
{
    return (type_ == FLOAT_SCALAR);
}

inline Foam::floatScalar Foam::token::floatScalarToken() const
{
    if (type_ == FLOAT_SCALAR)
    {
        return floatScalarToken_;
    }
    else
    {
        parseError("floatScalar");
        return 0.0;
    }
}


inline bool Foam::token::isDoubleScalar() const
{
    return (type_ == DOUBLE_SCALAR);
}

inline Foam::doubleScalar Foam::token::doubleScalarToken() const
{
    if (type_ == DOUBLE_SCALAR)
    {
        return doubleScalarToken_;
    }
    else
    {
        parseError("doubleScalar");
        return 0.0;
    }
}


inline bool Foam::token::isLongDoubleScalar() const
{
    return (type_ == LONG_DOUBLE_SCALAR);
}

inline Foam::longDoubleScalar Foam::token::longDoubleScalarToken() const
{
    if (type_ == LONG_DOUBLE_SCALAR)
    {
        return *longDoubleScalarTokenPtr_;
    }
    else
    {
        parseError("longDoubleScalar");
        return 0.0;
    }
}


inline bool Foam::token::isScalar() const
{
    return
        type_ == FLOAT_SCALAR
     || type_ == DOUBLE_SCALAR
     || type_ == LONG_DOUBLE_SCALAR;
}

inline Foam::scalar Foam::token::scalarToken() const
{
    if (type_ == FLOAT_SCALAR)
    {
        return floatScalarToken_;
    }
    else if (type_ == DOUBLE_SCALAR)
    {
        return doubleScalarToken_;
    }
    else if (type_ == LONG_DOUBLE_SCALAR)
    {
        return *longDoubleScalarTokenPtr_;
    }
    else
    {
        parseError(pTraits<scalar>::typeName);
        return 0.0;
    }
}

inline bool Foam::token::isNumber() const
{
    return
        type_ == INTEGER_32
     || type_ == INTEGER_64
     || type_ == UNSIGNED_INTEGER_32
     || type_ == UNSIGNED_INTEGER_64
     || isScalar();
}

inline Foam::scalar Foam::token::number() const
{
    if (type_ == INTEGER_32)
    {
        return integer32Token_;
    }
    else if (type_ == INTEGER_64)
    {
        return integer64Token_;
    }
    if (type_ == UNSIGNED_INTEGER_32)
    {
        return unsignedInteger32Token_;
    }
    else if (type_ == UNSIGNED_INTEGER_64)
    {
        return unsignedInteger64Token_;
    }
    else if (isScalar())
    {
        return scalarToken();
    }
    else
    {
        parseError("number (label or scalar)");
        return 0.0;
    }
}

inline bool Foam::token::isCompound() const
{
    return (type_ == COMPOUND);
}

inline const Foam::token::compound& Foam::token::compoundToken() const
{
    if (type_ == COMPOUND)
    {
        return *compoundTokenPtr_;
    }
    else
    {
        parseError("compound");
        return *compoundTokenPtr_;
    }
}


inline Foam::label Foam::token::lineNumber() const
{
    return lineNumber_;
}

inline Foam::label& Foam::token::lineNumber()
{
    return lineNumber_;
}


inline void Foam::token::setBad()
{
    clear();
    type_ = ERROR;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

inline void Foam::token::operator=(const token& t)
{
    clear();
    type_ = t.type_;

    switch (type_)
    {
        case token::UNDEFINED:
        break;

        case PUNCTUATION:
            punctuationToken_ = t.punctuationToken_;
        break;

        case WORD:
            wordTokenPtr_ = new word(*t.wordTokenPtr_);
        break;

        case FUNCTIONNAME:
            functionNameTokenPtr_ = new functionName(*t.functionNameTokenPtr_);
        break;

        case VARIABLE:
            variableTokenPtr_ = new variable(*t.variableTokenPtr_);
        break;

        case STRING:
            stringTokenPtr_ = new string(*t.stringTokenPtr_);
        break;

        case VERBATIMSTRING:
            verbatimStringTokenPtr_ =
                new verbatimString(*t.verbatimStringTokenPtr_);
        break;

        case INTEGER_32:
            integer32Token_ = t.integer32Token_;
        break;

        case INTEGER_64:
            integer64Token_ = t.integer64Token_;
        break;

        case UNSIGNED_INTEGER_32:
            unsignedInteger32Token_ = t.unsignedInteger32Token_;
        break;

        case UNSIGNED_INTEGER_64:
            unsignedInteger64Token_ = t.unsignedInteger64Token_;
        break;

        case FLOAT_SCALAR:
            floatScalarToken_ = t.floatScalarToken_;
        break;

        case DOUBLE_SCALAR:
            doubleScalarToken_ = t.doubleScalarToken_;
        break;

        case LONG_DOUBLE_SCALAR:
            longDoubleScalarTokenPtr_ =
                new longDoubleScalar(*t.longDoubleScalarTokenPtr_);
        break;

        case COMPOUND:
            compoundTokenPtr_ = t.compoundTokenPtr_;
            compoundTokenPtr_->refCount::operator++();
        break;

        case token::ERROR:
        break;
    }

    lineNumber_ = t.lineNumber_;
}

inline void Foam::token::operator=(const punctuationToken p)
{
    clear();
    type_ = PUNCTUATION;
    punctuationToken_ = p;
}

inline void Foam::token::operator=(word* wPtr)
{
    clear();
    type_ = WORD;
    wordTokenPtr_ = wPtr;
}

inline void Foam::token::operator=(const word& w)
{
    operator=(new word(w));
}

inline void Foam::token::operator=(functionName* fnPtr)
{
    clear();
    type_ = FUNCTIONNAME;
    functionNameTokenPtr_ = fnPtr;
}

inline void Foam::token::operator=(const functionName& fn)
{
    operator=(new functionName(fn));
}

inline void Foam::token::operator=(variable* vPtr)
{
    clear();
    type_ = VARIABLE;
    variableTokenPtr_ = vPtr;
}

inline void Foam::token::operator=(const variable& v)
{
    operator=(new variable(v));
}

inline void Foam::token::operator=(string* sPtr)
{
    clear();
    type_ = STRING;
    stringTokenPtr_ = sPtr;
}

inline void Foam::token::operator=(const string& s)
{
    operator=(new string(s));
}

inline void Foam::token::operator=(verbatimString* vsPtr)
{
    clear();
    type_ = VERBATIMSTRING;
    verbatimStringTokenPtr_ = vsPtr;
}

inline void Foam::token::operator=(const verbatimString& vs)
{
    operator=(new verbatimString(vs));
}

inline void Foam::token::operator=(const int32_t l)
{
    clear();
    type_ = INTEGER_32;
    integer32Token_ = l;
}

inline void Foam::token::operator=(const int64_t l)
{
    clear();
    type_ = INTEGER_64;
    integer64Token_ = l;
}

inline void Foam::token::operator=(const uint32_t l)
{
    clear();
    type_ = UNSIGNED_INTEGER_32;
    unsignedInteger32Token_ = l;
}

inline void Foam::token::operator=(const uint64_t l)
{
    clear();
    type_ = UNSIGNED_INTEGER_64;
    unsignedInteger64Token_ = l;
}

inline void Foam::token::operator=(const floatScalar s)
{
    clear();
    type_ = FLOAT_SCALAR;
    floatScalarToken_ = s;
}

inline void Foam::token::operator=(const doubleScalar s)
{
    clear();
    type_ = DOUBLE_SCALAR;
    doubleScalarToken_ = s;
}

inline void Foam::token::operator=(const longDoubleScalar s)
{
    clear();
    type_ = LONG_DOUBLE_SCALAR;
    longDoubleScalarTokenPtr_ = new longDoubleScalar(s);
}

inline void Foam::token::operator=(Foam::token::compound* cPtr)
{
    clear();
    type_ = COMPOUND;
    compoundTokenPtr_ = cPtr;
}


inline bool Foam::token::operator==(const token& t) const
{
    if (type_ != t.type_)
    {
        return false;
    }

    switch (type_)
    {
        case token::UNDEFINED:
            return true;

        case PUNCTUATION:
            return punctuationToken_ == t.punctuationToken_;

        case WORD:
            return *wordTokenPtr_ == *t.wordTokenPtr_;

        case FUNCTIONNAME:
            return *functionNameTokenPtr_ == *t.functionNameTokenPtr_;

        case VARIABLE:
            return *variableTokenPtr_ == *t.variableTokenPtr_;

        case STRING:
            return *stringTokenPtr_ == *t.stringTokenPtr_;

        case VERBATIMSTRING:
            return *verbatimStringTokenPtr_ == *t.verbatimStringTokenPtr_;

        case INTEGER_32:
            return integer32Token_ == t.integer32Token_;

        case INTEGER_64:
            return integer64Token_ == t.integer64Token_;

        case UNSIGNED_INTEGER_32:
            return unsignedInteger32Token_ == t.unsignedInteger32Token_;

        case UNSIGNED_INTEGER_64:
            return unsignedInteger64Token_ == t.unsignedInteger64Token_;

        case FLOAT_SCALAR:
            return equal(floatScalarToken_, t.floatScalarToken_);

        case DOUBLE_SCALAR:
            return equal(doubleScalarToken_, t.doubleScalarToken_);

        case LONG_DOUBLE_SCALAR:
            return equal
            (
                *longDoubleScalarTokenPtr_,
                *t.longDoubleScalarTokenPtr_
            );

        case COMPOUND:
            return compoundTokenPtr_ == t.compoundTokenPtr_;

        case token::ERROR:
            return true;
    }

    return false;
}

inline bool Foam::token::operator==(const punctuationToken p) const
{
    return (type_ == PUNCTUATION && punctuationToken_ == p);
}

inline bool Foam::token::operator==(const word& w) const
{
    return (type_ == WORD && wordToken() == w);
}

inline bool Foam::token::operator==(const functionName& fn) const
{
    return (type_ == FUNCTIONNAME && functionNameToken() == fn);
}

inline bool Foam::token::operator==(const variable& v) const
{
    return (type_ == VARIABLE && variableToken() == v);
}

inline bool Foam::token::operator==(const string& s) const
{
    return (type_ == STRING && stringToken() == s);
}

inline bool Foam::token::operator==(const verbatimString& vs) const
{
    return (type_ == VERBATIMSTRING && verbatimStringToken() == vs);
}

inline bool Foam::token::operator==(const int32_t l) const
{
    return (type_ == INTEGER_32 && integer32Token_ == l);
}

inline bool Foam::token::operator==(const int64_t l) const
{
    return (type_ == INTEGER_64 && integer64Token_ == l);
}

inline bool Foam::token::operator==(const uint32_t l) const
{
    return (type_ == UNSIGNED_INTEGER_32 && unsignedInteger32Token_ == l);
}

inline bool Foam::token::operator==(const uint64_t l) const
{
    return (type_ == UNSIGNED_INTEGER_64 && unsignedInteger64Token_ == l);
}

inline bool Foam::token::operator==(const floatScalar s) const
{
    return (type_ == FLOAT_SCALAR && equal(floatScalarToken_, s));
}

inline bool Foam::token::operator==(const doubleScalar s) const
{
    return (type_ == DOUBLE_SCALAR && equal(doubleScalarToken_, s));
}

inline bool Foam::token::operator==(const longDoubleScalar s) const
{
    return
    (
        type_ == LONG_DOUBLE_SCALAR && equal(*longDoubleScalarTokenPtr_, s)
    );
}

inline bool Foam::token::operator!=(const token& t) const
{
    return !operator==(t);
}

inline bool Foam::token::operator!=(const punctuationToken p) const
{
    return !operator==(p);
}

inline bool Foam::token::operator!=(const word& w) const
{
    return !operator==(w);
}

inline bool Foam::token::operator!=(const functionName& fn) const
{
    return !operator==(fn);
}

inline bool Foam::token::operator!=(const variable& v) const
{
    return !operator==(v);
}

inline bool Foam::token::operator!=(const string& s) const
{
    return !operator==(s);
}

inline bool Foam::token::operator!=(const verbatimString& vs) const
{
    return !operator==(vs);
}

inline bool Foam::token::operator!=(const int32_t l) const
{
    return !operator==(l);
}

inline bool Foam::token::operator!=(const int64_t l) const
{
    return !operator==(l);
}

inline bool Foam::token::operator!=(const uint32_t l) const
{
    return !operator==(l);
}

inline bool Foam::token::operator!=(const uint64_t l) const
{
    return !operator==(l);
}

inline bool Foam::token::operator!=(const floatScalar s) const
{
    return !operator==(s);
}

inline bool Foam::token::operator!=(const doubleScalar s) const
{
    return !operator==(s);
}

inline bool Foam::token::operator!=(const longDoubleScalar s) const
{
    return !operator==(s);
}


// ************************************************************************* //
