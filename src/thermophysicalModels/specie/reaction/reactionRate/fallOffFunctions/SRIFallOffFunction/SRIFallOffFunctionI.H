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

#include "SRIFallOffFunction.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::SRIFallOffFunction::SRIFallOffFunction
(
    const scalar a,
    const scalar b,
    const scalar c,
    const scalar d,
    const scalar e
)
:
    a_(a),
    b_(b),
    c_(c),
    d_(d),
    e_(e)
{}


inline Foam::SRIFallOffFunction::SRIFallOffFunction(const dictionary& dict)
:
    a_(dict.lookup<scalar>("a")),
    b_(dict.lookup<scalar>("b")),
    c_(dict.lookup<scalar>("c")),
    d_(dict.lookup<scalar>("d")),
    e_(dict.lookup<scalar>("e"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline Foam::scalar Foam::SRIFallOffFunction::operator()
(
    const scalar T,
    const scalar Pr
) const
{
    const scalar logPr = log10(max(Pr, small));

    const scalar X = 1/(1 + sqr(logPr));

    const scalar psi = a_*exp(-b_/T) + exp(-T/c_);

    return d_*pow(psi, X)*pow(T, e_);
}


inline Foam::scalar Foam::SRIFallOffFunction::ddT
(
    const scalar T,
    const scalar Pr,
    const scalar F
) const
{
    const scalar logPr = log10(max(Pr, small));

    const scalar X = 1/(1 + sqr(logPr));

    const scalar psi = a_*exp(-b_/T) + exp(-T/c_);
    const scalar dpsidT = a_*b_/sqr(T)*exp(-b_/T) - 1/c_*exp(-T/c_);

    return F*(X/psi*dpsidT + e_/T);
}


inline Foam::scalar Foam::SRIFallOffFunction::ddPr
(
    const scalar T,
    const scalar Pr,
    const scalar F
) const
{
    static const scalar logTen = log(scalar(10));

    const scalar logPr = log10(max(Pr, small));
    const scalar dlogPrdPr = Pr >= small ? 1/(logTen*Pr) : 0;

    const scalar X = 1/(1 + sqr(logPr));
    const scalar dXdPr = -sqr(X)*2*logPr*dlogPrdPr;

    const scalar psi = a_*exp(-b_/T) + exp(-T/c_);

    return F*log(psi)*dXdPr;
}


inline void Foam::SRIFallOffFunction::write(Ostream& os) const
{
    writeEntry(os, "a", a_);
    writeEntry(os, "b", b_);
    writeEntry(os, "c", c_);
    writeEntry(os, "d", d_);
    writeEntry(os, "e", e_);
}


// * * * * * * * * * * * * * * * Ostream Operator  * * * * * * * * * * * * * //

inline Foam::Ostream& Foam::operator<<
(
    Foam::Ostream& os,
    const Foam::SRIFallOffFunction& srifof
)
{
    srifof.write(os);
    return os;
}


// ************************************************************************* //
